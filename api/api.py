from __future__ import print_function, division, absolute_import
from flask import Flask
from flask_cors import CORS

import serial
import time
from robot_serial import RobotOrder, write_order, write_i8, read_i8, read_order, write_color

app = Flask(__name__)
CORS(app)
global is_connected
global serial_file
is_connected = False

try:
  serial_file = serial.Serial(port='/dev/ttyUSB0', baudrate=9600, timeout=5, writeTimeout=0)
except Exception as e:
  raise e

def wait_for_connect():
  # Initialize communication with Arduino
  global serial_file
  global is_connected
  while not is_connected:
    print("Waiting for arduino connection...")
    write_order(serial_file, RobotOrder.HELLO)
    bytes_array = bytearray(serial_file.read(1))
    if not bytes_array:
      time.sleep(2)
      continue
    byte = bytes_array[0]
    print("Byte: {}", byte)
    if byte in [RobotOrder.HELLO.value, RobotOrder.ALREADY_CONNECTED.value]:
      is_connected = True
    print("Connected to Arduino")
    read_all_available()

def read_all_available():
  global serial_file
  while serial_file.in_waiting > 0:
    cmd = read_order(serial_file)
    print("Read additional command:", cmd)

wait_for_connect()
	
@app.route("/")
def hello():
  return "Welcome to dj robot!"

@app.route('/<order>/<param>')
def command(order, param):
  global serial_file
  global is_connected
  if is_connected:
    try:
	  current_order = RobotOrder(int(order))
    except:
      return '{"status": "ERROR", "msg":"Invalid command."}'
	  
    write_order(serial_file, RobotOrder(int(order)))
	
    if current_order in [RobotOrder.LEFT_EYE_COLOR, RobotOrder.RIGHT_EYE_COLOR]:
        print("color sent in- sending.")
        write_color(serial_file, param)

    response = read_order(serial_file)
	
    if not response:
      is_connected = False
      return '{"status": "ERROR", "msg":"Lost connection with the arduino."}'
    elif response == RobotOrder.RECEIVED:
      return '{"status": "SUCCESS", "msg":"Command run successfully!"}'
    else:
      return '{"status": "ERROR", "msg":"The arduino failed to run your command."}'
  else:
    return '{"status": "ERROR", "msg":"Arduino is not connected."}'
