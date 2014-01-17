from __future__ import print_function

import uuid


class Node(object):
    guid = uuid.uuid1()

    def __init__(self, name):
        self.name = name

    def connect_to(self):
        pass

    def map_to(self):
        pass
