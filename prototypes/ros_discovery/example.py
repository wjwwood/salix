#!/usr/bin/env python

import os
import sys
sys.path.append(os.path.join(os.path.dirname(__file__), 'src'))

from ros_discovery.msgs.advertise import AdvertiseMessage
from ros_discovery.msgs.advertise import pack_advertise_msg
from ros_discovery.msgs.advertise import unpack_advertise_msg

adv = AdvertiseMessage('/foo', 'tcp://192.168.1.10:51168')
data = pack_advertise_msg(adv)
print(unpack_advertise_msg(data))
