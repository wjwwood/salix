#!/usr/bin/env python

from __future__ import print_function

import sys

from ros_discovery.graph import Graph
from ros_discovery.impl.distributed import DistributedTransportGraphImpl
from ros_discovery.msgs import node_started_msg_from_node
from ros_discovery.node import Node


def main():
    dg = DistributedTransportGraphImpl()
    g = Graph(dg)
    n = Node(sys.argv[0])
    msg = node_started_msg_from_node(n)
    g.publish(msg)


if __name__ == '__main__':
    main()
