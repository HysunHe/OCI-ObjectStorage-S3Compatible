""" Hysun He (hysun.he@oracle.com) @ 2023/07/06 """

import os
import base64
import hashlib
import logging
import subprocess
from pathlib import Path
from oci_config import OciConf as oci_conf
from devlog import my_logger as log_utils

logger = logging.getLogger(__name__)

WORK_DIR = os.environ.get('BUF_FILE_DIR', '/var/tmp/ossappend')


def ensure_file_exists(file_name: str):
    """ docstring """
    if os.path.isfile(file_name):
        return  # file already exists
    ensure_file_dir_exists(file_name)
    Path(file_name).touch()


def ensure_file_dir_exists(file_fullname: str):
    """ docstring """
    file_path = Path(file_fullname)
    file_dir = file_path.parent.absolute()
    if os.path.isdir(file_dir):
        return  # dir already exists
    os.makedirs(name=file_dir, exist_ok=True)


def delete_file(file_name: str):
    """ docstring """
    if not os.path.isfile(file_name):
        return  # file not exists
    try:
        os.remove(file_name)
    except OSError:
        pass


def sync_object_storage(bucket_name: str, src_file: str, dest_file: str):
    """Upload a file to OCI Object Storage."""
    logger.debug('Sync file %s ==> %s', src_file, dest_file)
    upload_via_ocicli = (f'oci os object put --bucket-name {bucket_name} '
                         f'--name "{dest_file}" --file "{src_file}" --force')
    try:
        # with log_utils.safe_rich_status(
        #         f'[bold cyan]Uploading file {src_file}[/]'):
        subprocess.check_output(upload_via_ocicli, shell=True)
        delete_file(file_name=src_file)
        logger.debug('Local file %s...deleted', src_file)
    except subprocess.CalledProcessError as ex:
        logger.error(ex.output)
        with log_utils.print_exception_no_traceback():
            raise IOError(f'Failed to upload file {src_file}.') from ex


def gen_auth_md5(datetime: str) -> str:
    """ auth header """
    access_key = oci_conf.get_accesskey()
    secret_key = oci_conf.get_secretkey()
    md5_str = hashlib.md5(
        f'{access_key}{secret_key}{datetime}'.encode()).digest()
    base64_str = base64.b64encode(md5_str).decode()
    return base64_str
