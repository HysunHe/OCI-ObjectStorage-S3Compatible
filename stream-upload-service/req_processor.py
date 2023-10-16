""" Hysun He (hysun.he@oracle.com) @ 2023/07/04 """

import logging
import base64

from flask import Flask, request
from gevent import pywsgi

import my_utils
import env_config
from devlog import my_logger as log_utils
from oci_config import OciConf as oci_conf
from task_queue import TqMgr

logger = logging.getLogger(__name__)
app = Flask(__name__)


@app.route('/write-json', methods=['POST'])
def write_json():
    """ JSON payload """
    auth_result = authorize_request(request)
    if auth_result is not None:
        return auth_result

    data = request.get_json(force=True)
    bucket = data['bucket']
    file_name = data['name']
    destination = file_name
    file_position = int('0' if 'position' not in data else data['position'])
    whence = 2 if 'position' not in data or file_position < 0 else 0
    content_base64 = data['content']
    append = data['append']
    content_bytes = base64.b64decode(content_base64)
    logger.debug('file_name: %s', file_name)
    logger.debug('file_position: %d, %d', file_position, whence)
    logger.debug('append: %s', append)

    pos = handle_content(file_name=file_name,
                         file_position=file_position,
                         whence=whence,
                         content_bytes=content_bytes,
                         append=append,
                         bucket=bucket,
                         destination=destination)

    # pylint: disable=line-too-long
    location = f'https://objectstorage.{oci_conf.get_region()}.oraclecloud.com/n/{oci_conf.get_namespace()}/b/{bucket}/o/{file_name}' if append and str(
        append).lower() not in ('true', '1') else ''
    return {'status': 'ok', 'current_file_position': pos, 'location': location}


@app.route('/write-bytes', methods=['POST'])
def write_bytes():
    """ Bytes payload """
    auth_result = authorize_request(request)
    if auth_result is not None:
        return auth_result

    content_bytes = request.get_data()
    bucket = request.args.get('bucket')
    file_name = request.args.get('name')
    append = request.args.get('append')
    file_position = int('0' if not request.args.get('position') else request.
                        args.get('position'))
    destination = file_name
    whence = 2 if not request.args.get('position') or file_position < 0 else 0
    logger.debug('file_name: %s', file_name)
    logger.debug('file_position: %d, %d', file_position, whence)
    logger.debug('append: %s', append)

    pos = handle_content(file_name=file_name,
                         file_position=file_position,
                         whence=whence,
                         content_bytes=content_bytes,
                         append=append,
                         bucket=bucket,
                         destination=destination)

    # pylint: disable=line-too-long
    location = f'https://objectstorage.{oci_conf.get_region()}.oraclecloud.com/n/{oci_conf.get_namespace()}/b/{bucket}/o/{file_name}' if append and str(
        append).lower() not in ('true', '1') else ''
    return {'status': 'ok', 'current_file_position': pos, 'location': location}


def authorize_request(req):
    """ docstring """
    headers = req.headers
    x_amz_date = headers.get('X-Amz-Date')
    assert x_amz_date is not None, 'Mising required header: X-Amz-Date'
    authorization = headers.get('Authorization')
    assert authorization is not None, 'Mising required header: Authorization'
    auth_local = my_utils.gen_auth_md5(x_amz_date)
    if authorization != auth_local:
        return 'Unauthorized', 401
    return None


# pylint: disable=too-many-arguments
def handle_content(file_name, file_position, whence, content_bytes, append,
                   bucket, destination) -> int:
    """ docstring """
    file_fullname = f'{my_utils.WORK_DIR}/{file_name}'
    my_utils.ensure_file_exists(file_fullname)

    with open(file_fullname, 'rb+') as dest_file:
        logger.debug('Write content to file. file_position: %d, %d',
                     file_position, whence)
        dest_file.seek(0 if file_position < 0 else file_position, whence)
        dest_file.write(content_bytes)
        current_position = dest_file.tell()

    if append and str(append).lower() not in ('true', '1'):
        TqMgr.inst().enqueue(task_tuple=(bucket, file_fullname, destination))

    return current_position


@log_utils.debug_enabled(logger)
def run():
    """ docstring """
    # app.run(host='0.0.0.0') # Dev mode
    host = env_config.SERVER_HOST
    port = env_config.SERVER_LISTEN_PORT
    server = pywsgi.WSGIServer((host, port), app)
    server.serve_forever()
