import socket
import struct

from ros_discovery.msg import Msg


class DistributedTransportGraphImpl(object):
    # Multicast Addr: 255. R. O. S
    multicast_addr = '225.82.79.83'
    multicast_port = '11311'

    def __init__(self):
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
        self.sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.sock.bind(('', self.multicast_port))
        mreq = struct.pack("4sl", socket.inet_aton(self.multicast_addr), socket.INADDR_ANY)

        self.sock.setsockopt(socket.IPPROTO_IP, socket.IP_ADD_MEMBERSHIP, mreq)

        self.sock.setsockopt(socket.IPPROTO_IP, socket.IP_MULTICAST_TTL, 2)

    def recv_msg(self):
        msg = Msg()
        msg.unpack(self.sock.recv(Msg.msg_size))
        return msg

    def send_msg(self, msg):
        return self.sock.sendto(msg.pack(), (self.multicast_addr, self.multicast_port))
