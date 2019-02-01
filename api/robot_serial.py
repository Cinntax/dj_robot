from __future__ import print_function, division, unicode_literals, absolute_import

import struct

from enum import Enum

class RobotOrder(Enum):
    """
    Pre-defined orders
    """
    HELLO = 0
    DISCO_BALL = 1
    LEFT_EYE_COLOR = 2
    RIGHT_EYE_COLOR = 3
    LEFT_ARM = 4
    RIGHT_ARM = 5
    FORWARD = 6
    BACKWARD = 7
    TURN_LEFT = 8
    TURN_RIGHT = 9
    ALREADY_CONNECTED = 10
    ERROR = 11
    RECEIVED = 12
    STOP = 13
    DANCE = 14
	
def read_order(f):
    """
    :param f: file handler or serial file
    :return: (Order Enum Object)
    """
    return RobotOrder(struct.unpack('<B', bytearray(f.read(1)))[0])


def write_order(f, order, data):
    """
	Writes the order and any arguments afterward.
	"""
	#Write the order.
    f.write(struct.pack('<B', order.value))

	#Write the following data/arguments.	
    f.write(struct.pack("<%uB" % len(data), *data))

def get_color(color_value):
    r = color_value[:2]
    g = color_value[2:4]
    b = color_value[4:6]
    return [int(r, 16), int(g, 16), int(b, 16)]
