from __future__ import print_function, division, absolute_import
from flask import Flask
from flask_cors import CORS

import serial
import time
import subprocess
import random
import threading

from robot_serial import RobotOrder, write_order, read_order, get_color

app = Flask(__name__)
CORS(app)
global is_connected
global serial_file
global player_process
player_process = None
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
    write_order(serial_file, RobotOrder.HELLO, [])
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
  return app.send_static_file('client.html')

@app.route('/<int:order>/<param>')
def command(order, param):
  global serial_file
  global is_connected
  global player_process
  if is_connected:
    try:
      print(param)  
      current_order = RobotOrder(int(order))
    except:
      return '{"status": "ERROR", "msg":"Invalid command."}'
	  
    if current_order == RobotOrder.DANCE: #We want to run this on the RPI, and not the arduino.
      print("playing music...")
      if player_process is None:
        player_process = subprocess.Popen(['/usr/bin/play', get_random_song()])
        dance()

    elif current_order == RobotOrder.STOP:
      try:
        write_order(serial_file, RobotOrder(int(order)), [])
        player_process.terminate()
        player_process = None
      except:
        print("No music playing- not stopping.")  

    elif current_order in [RobotOrder.LEFT_EYE_COLOR, RobotOrder.RIGHT_EYE_COLOR]:
      print("color sent in- sending.")
      write_order(serial_file, RobotOrder(int(order)), get_color(param))
    else:
      data = [int(numeric_string) for numeric_string in param.split("|")]
      write_order(serial_file, RobotOrder(int(order)), data)
    
    response = read_order(serial_file)
	
    if not response:
      is_connected = False
      return '{"status": "ERROR", "msg":"Lost connection with the arduino."}'
    elif response == RobotOrder.RECEIVED:
      return '{"status": "SUCCESS", "msg":"Command run successfully!"}'
    else:
      return '{"status": "ERROR", "msg":"The arduino failed to run your command."}'
    
    return '{"status": "SUCCESS", "msg":"Command run successfully!"}'
 
  else:
    return '{"status": "ERROR", "msg":"Arduino is not connected."}'
	
def get_random_song():
  song_number = random.randint(0,4)
  songs = ['getlucky.mp3', 'cheerleader.mp3', 'partyrockers.mp3', 'shake.mp3','lifehighway.mp3']
  return '/home/pi/music/' + songs[song_number]

def get_random_eye_color():
  color1 = ''.join('{:02X}'.format(random.randint(1,254)))
  color2 = ''.join('{:02X}'.format(random.randint(1,254)))
  color3 = ''.join('{:02X}'.format(random.randint(1,254)))
  return color1 + color2 + color3
  
def dance():
  if player_process is not None:
    write_order(serial_file, RobotOrder.DISCO_BALL, [1])
    eye_color = get_random_eye_color()
    left_arm_speed = random.randint(60,100)
    right_arm_speed = random.randint(60,100)
    left_arm_direction = random.randint(0,1)
    right_arm_direction = random.randint(0,1)
    turn_direction = random.randint(1,100)
  
    write_order(serial_file, RobotOrder.LEFT_EYE_COLOR, get_color(eye_color))	
    write_order(serial_file, RobotOrder.RIGHT_EYE_COLOR, get_color(eye_color))	
    write_order(serial_file, RobotOrder.LEFT_ARM, [left_arm_direction,left_arm_speed])
    write_order(serial_file, RobotOrder.RIGHT_ARM, [right_arm_direction,right_arm_speed])

    if turn_direction < 30:
      write_order(serial_file, RobotOrder.FORWARD, [0])
    elif turn_direction >= 31 and turn_direction < 65:
      write_order(serial_file, RobotOrder.TURN_LEFT, [100])
    else:
      write_order(serial_file, RobotOrder.TURN_RIGHT, [100])
    
    threading.Timer(0.5, dance).start()
