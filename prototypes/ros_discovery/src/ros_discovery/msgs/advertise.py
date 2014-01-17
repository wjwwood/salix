from __future__ import print_function

import struct

from ros_discovery.msgs.header import pack_message_header
from ros_discovery.msgs.header import unpack_message_header
from ros_discovery.msgs.header import MessageHeader

ADV_OP = 0x01

# format: little endian, addre_len:H, addr:{0-267}s
advertise_pack_format = '<H{0}s'


def pack_advertise_msg(msg):
    header_data = pack_message_header(msg)
    fmt = advertise_pack_format.format(len(msg.address))
    return header_data + struct.pack(fmt, len(msg.address), msg.address)


def unpack_advertise_msg(data, header=None):
    # Unpack header if not passed
    if header is None:
        header, header_len = unpack_message_header(data)
    else:
        header_len = header.size()
    # Calculate the address len
    addr_len = struct.unpack_from('<H', data, header_len)[0]
    # Unpack the address
    _, address = struct.unpack_from(advertise_pack_format.format(addr_len), data, header_len)
    # Create the Advertise Message
    adv_msg = AdvertiseMessage(header.topic, address)
    adv_msg.set_header(header)
    return adv_msg


class AdvertiseMessage(MessageHeader):
    def __init__(self, topic, address):
        super(AdvertiseMessage, self).__init__(topic, ADV_OP)
        if len(address) > 267:
            raise ValueError(
                "Address exceeds the maximum address lenth of 267, " +
                "with length '{0}': '{1}'".format(len(address), address)
            )
        self.address = address

    def set_header(self, header):
        self.topic = header.topic
        self.type = header.type
        self.flags = header.flags
        self.guid = header.guid
        self.version = header.version

    def size(self):
        return len(pack_advertise_msg(self))

    def __str__(self):
        return ("<AdvertiseMessage"
                " topic='{topic}', address='{address}',"
                " msg_type=0x{type:02x},"
                " flags={flags}, guid={guid},"
                " version=0x{version:02x}>").format(**self.__dict__)
