import signal
import time

ros_running = True


def signal_handler(signum, frame):
    global ros_running
    ros_running = False

signal.signal(signal.SIGINT, signal_handler)


def ok():
    global ros_running
    return ros_running


class Rate(object):
    def __init__(self, rate):
        self.rate = rate
        self.period = 1.0 / rate
        self.__next_epoch = None

    def sleep(self):
        current_time = time.time()
        if self.__next_epoch is None:
            self.__next_epoch = current_time + self.period
        if current_time < self.__next_epoch:
            time.sleep(self.__next_epoch - current_time)
        self.__next_epoch += self.period
