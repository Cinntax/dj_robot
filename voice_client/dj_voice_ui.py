import snowboydecoder
import sys
import signal
import requests
import threading
import subprocess

interrupted = False
awaiting_command = False

stop_url = 'http://localhost:5000/13/0%7C0'
left_eye_url = 'http://localhost:5000/2/00ff00%7C0'
right_eye_url = 'http://localhost:5000/3/00ff00%7C0'
dance_url = 'http://localhost:5000/14/0%7C0'

def abort_command_callback():
    global awaiting_command
    global stop_url
    if awaiting_command == True:
        awaiting_command = False
        requests.get(stop_url)

abort_timer = threading.Timer(5.0, abort_command_callback)

def signal_handler(signal, frame):
    global interrupted
    interrupted = True


def interrupt_callback():
    global interrupted
    return interrupted

def dj_callback():
    global awaiting_command
    global stop_url
    global left_eye_url
    global right_eye_url
    global abort_timer

    if awaiting_command == False:
        print('DJ Called')
        requests.get(stop_url)
        requests.get(left_eye_url)
        requests.get(right_eye_url)
        awaiting_command = True
        subprocess.Popen(['/usr/bin/play', '/home/pi/dj_robot/voice_client/resources/whatcanido.mp3'])
        abort_timer = threading.Timer(5.0, abort_command_callback)
        abort_timer.start()
    else:
        print('DJ hotword ignored. Robot not in proper state.')

def stop_callback():
    global awaiting_command
    global stop_url
    global abort_timer

    if awaiting_command == True:
        print('Stop Called')
        subprocess.Popen(['/usr/bin/play', '/home/pi/dj_robot/voice_client/resources/ok.mp3']) 
        requests.get(stop_url)
        awaiting_command = False
        abort_timer.cancel()
    else:
        print('Stop hotword ignored. Robot not in proper state.')

def dance_callback():
    global awaiting_command
    global dance_url
    global abort_timer

    if awaiting_command == True:
        print('Dance Called')
        subprocess.Popen(['/usr/bin/play', '/home/pi/dj_robot/voice_client/resources/ok.mp3'])
        requests.get(dance_url)
        awaiting_command = False
        abort_timer.cancel()
    else:
        print('Dance hotword ignored. Robot not in proper state.')

models = ['/home/pi/dj_robot/voice_client/resources/DJ.pmdl','/home/pi/dj_robot/voice_client/resources/Stop.pmdl','/home/pi/dj_robot/voice_client/resources/Dance.pmdl']
sensitivity = [0.5]*len(models)
callbacks = [dj_callback, stop_callback, dance_callback]

# capture SIGINT signal, e.g., Ctrl+C
signal.signal(signal.SIGINT, signal_handler)

detector = snowboydecoder.HotwordDetector(models, sensitivity=sensitivity)
print('Listening... Press Ctrl+C to exit')

# main loop
detector.start(detected_callback=callbacks,
               interrupt_check=interrupt_callback,
               sleep_time=0.03)

detector.terminate()
