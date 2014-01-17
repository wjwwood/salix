#!/usr/bin/env python

import env
env.activate()

import ros2
from ros2_std_msgs.messages import String


def main():
    ros2.init_node('pub')
    publisher = ros2.Publisher('/chatter', String)

    rate = ros2.Rate(2)

    count = 0
    while ros2.ok and count != 10:
        publisher.publish(String('Hello World: ' + str(count + 1)))
        count += 1
        rate.sleep()

if __name__ == '__main__':
    main()
