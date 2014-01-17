from __future__ import print_function


class Graph(object):
    def __init__(self, impl):
        self.__nodes = []
        self.__impl = impl

    @property
    def nodes(self):
        return self.__nodes

    def publish(self, msg):
        self.__impl.send_msg(msg)
