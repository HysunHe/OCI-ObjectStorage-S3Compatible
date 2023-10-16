""" Hysun He (hysun.he@oracle.com) @ 2023/08/01 """

import os

OCI_CONFIG_FILE = os.environ.get('OCI_CONFIG_FILE', '~/.oci/config')

WORK_DIR = os.environ.get('BUF_FILE_DIR', '/var/tmp/ossappend')

MAX_QUEUE_SIZE = int(os.environ.get('MAX_QUEUE_SIZE', 0))

OSS_BUCKET = os.environ.get('OSS_BUCKET', None)

NO_UPDATE_TIMEOUT_SECONDS = int(os.environ.get('NO_UPDATE_TIMEOUT_SECONDS',
                                               30))

SERVER_HOST = os.environ.get('SERVER_HOST', '0.0.0.0')

SERVER_LISTEN_PORT = int(os.environ.get('SERVER_LISTEN_PORT', 5000))

SYNCER_DOP = int(os.environ.get('SYNCER_DOP', 0))

DEBUG_LEVEL = int(os.environ.get('DEBUG_LEVEL', 20))
