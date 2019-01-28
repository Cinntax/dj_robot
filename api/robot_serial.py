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
	
def read_order(f):
    """
    :param f: file handler or serial file
    :return: (Order Enum Object)
    """
    return RobotOrder(read_i8(f))

def read_i8(f):
    """
    :param f: file handler or serial file
    :return: (int8_t)
    """
    return struct.unpack('<b', bytearray(f.read(1)))[0]
	
def write_order(f, order):
    """
    :param f: file handler or serial file
    :param order: (Order Enum Object)
    """
    write_i8(f, order.value)
	
def write_i8(f, value):
    """
    :param f: file handler or serial file
    :param value: (int8_t)
    """
    if -128 <= value <= 127:
        f.write(struct.pack('<b', value))
    else:
        print("Value error:{}".format(value))

def write_color(f, value):
    """
    :param f: file handler or serial file
    :param value: a 3 byte hex string that contains the RGB values of the color to send. For example ffe033
    """
    f.write(value.decode("hex"))