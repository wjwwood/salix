from __future__ import print_function

import struct
import uuid

# format: little endian, version:H, GUID:16s, Topic:{0-192}s, Type:B, Flags:16B
header_pack_format = '<H16sB{0}sB16B'
topic_len_offset = struct.calcsize(header_pack_format.split('B')[0])


def pack_message_header(msg):
    args = [
        msg.version,
        msg.guid.bytes_le,
        len(msg.topic),
        msg.topic,
        msg.type
    ] + msg.flags
    return struct.pack(header_pack_format.format(len(msg.topic)), *args)


def unpack_message_header(data):
    topic_len = struct.unpack_from('<B', data, topic_len_offset)[0]
    fmt = header_pack_format.format(topic_len)
    ud = struct.unpack(fmt, data[:struct.calcsize(fmt)])
    guid = uuid.UUID(bytes_le=ud[1])
    msg = MessageHeader(ud[3], ud[4], ud[5:], guid, ud[0])
    return msg, struct.calcsize(fmt)


class MessageHeader(object):
    def __init__(self, topic, msg_type, flags=None, guid=None, version=0x01):
        if len(topic) > 192:
            raise ValueError(
                "Topic exceeds the maximum topic length of 192, " +
                "with length '{0}': '{1}'".format(len(topic), topic)
            )
        self.topic = topic
        self.type = msg_type
        self.flags = [0x0] * 16 if flags is None else flags
        self.guid = uuid.UUID(bytes_le='\x00' * 16) if guid is None else guid
        self.version = version

    def size(self):
        return len(pack_message_header(self))

    def __str__(self):
        return ("<MessageHeader "
                "topic='{topic}', msg_type=0x{type:02x}, "
                "flags={flags}, guid={guid}, "
                "version=0x{version:02x}>").format(**self.__dict__)
