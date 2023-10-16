""" Hysun He (hysun.he@oracle.com) @ 2023/08/01 """

import queue
import threading
import env_config


class TqMgr:
    """ docstring """
    _instance = None
    _lock = threading.RLock()

    def __init__(self):
        """ docstring """
        self._task_queue = queue.Queue(env_config.MAX_QUEUE_SIZE)

    @classmethod
    def inst(cls):
        """ docstring """
        with cls._lock:
            if cls._instance is None:
                super()
                cls._instance = super(TqMgr, cls).__new__(cls)
                cls._instance.__init__()
        return cls._instance

    def enqueue(self, task_tuple: tuple[str, str, str]):
        """ docstring """
        self._task_queue.put(item=task_tuple, block=True)

    def poll(self) -> tuple[str, str, str]:
        """ docstring """
        return self._task_queue.get(block=True)
