""" Hysun He (hysun.he@oracle.com) @ 2023/07/06 """
import os
from threading import Thread

import cleaner
import req_processor
import file_syncer


def run():
    """ docstring """
    bucket = os.environ.get('OSS_BUCKET')
    timeout = os.environ.get('NO_UPDATE_TIMEOUT_SECONDS')
    if not bucket:
        raise ValueError('Missing required env variable [OSS_BUCKET]')

    syncer_thread = Thread(target=file_syncer.run, daemon=True)
    syncer_thread.start()

    cleaner_thread = Thread(target=cleaner.run,
                            args=(
                                bucket,
                                timeout,
                            ),
                            daemon=True)
    cleaner_thread.start()

    req_processor.run()


if __name__ == '__main__':
    run()
