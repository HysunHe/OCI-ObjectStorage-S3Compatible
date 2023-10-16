""" Hysun He (hysun.he@oracle.com) @ 2023/07/06 """

import os
import time
import logging
from pathlib import Path
import my_utils
from task_queue import TqMgr

logger = logging.getLogger(__name__)

_SCAN_DIR = my_utils.WORK_DIR


def cleanup(bucket, timeout):
    """ docstring """
    for (root_dir, dirs, files) in os.walk(_SCAN_DIR):
        for file in files:
            process_file_upload(root_dir, file, bucket, timeout)
        for sub_dir in dirs:
            process_dir_cleanup(root_dir, sub_dir, timeout)


def process_file_upload(root_dir, file, bucket, timeout):
    """ docstring """
    file_path = os.path.join(root_dir, file)
    try:
        diff_seconds = int(time.time() - Path(file_path).stat().st_mtime)
        if diff_seconds >= int(timeout):
            dest_path = Path(file_path).relative_to(_SCAN_DIR)
            TqMgr.inst().enqueue(task_tuple=(bucket, file_path, dest_path))
    except FileNotFoundError:
        logger.debug('process_file_upload:: FileNotFoundError is OK')
    except IOError as err:
        logger.error('Error: %s: %s', file_path, str(err))
    except Exception as err:  # pylint: disable=broad-except
        logger.error('Unexpected error: %s', str(err))


def process_dir_cleanup(root_dir, sub_dir, timeout):
    """ docstring """
    dir_path = os.path.join(root_dir, sub_dir)
    try:
        diff_seconds = int(time.time() - Path(dir_path).stat().st_mtime)
        if diff_seconds >= int(timeout):
            if not os.listdir(dir_path):
                os.rmdir(dir_path)
    except FileNotFoundError:
        logger.debug('process_dir_cleanup:: FileNotFoundError is OK')
    except IOError as err:
        logger.error('Error: %s: %s', dir_path, str(err))
    except Exception as err:  # pylint: disable=broad-except
        logger.error('Unexpected error: %s', str(err))


def run(bucket, timeout):
    """ docstring """
    while True:
        cleanup(bucket, timeout)
        time.sleep(2)
