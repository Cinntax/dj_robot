#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <Wire.h>
#include "order.h"
#include "serial.h"

#define SERIAL_BAUD 9600  // Baudrate

#define WHEEL_PWMA  3
#define WHEEL_IN1  2
#define WHEEL_IN2  4
#define WHEEL_IN3  7
#define WHEEL_IN4  8
#define WHEEL_PWMB  5

#define ARM_PWMA  6
#define ARM_IN1  A5
#define ARM_IN2  A4
#define ARM_IN3  A3
#define ARM_IN4  A2
#define ARM_PWMB  9

#define LEFT_EYE  10
#define RIGHT_EYE  12

#define TOP_SIGNAL  11
#define TOP_POWER  A0

#define NUM_PIXELS  7

#define DEBUG false

Adafruit_NeoPixel pixelsLeft = Adafruit_NeoPixel(NUM_PIXELS, LEFT_EYE, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel pixelsRight = Adafruit_NeoPixel(NUM_PIXELS, RIGHT_EYE, NEO_GRBW + NEO_KHZ800);

bool is_connected = false;

void move(bool direction, int speed) {

  if(direction) {
    digitalWrite(WHEEL_IN1, HIGH);
    digitalWrite(WHEEL_IN3, HIGH);

    digitalWrite(WHEEL_IN2, LOW);
    digitalWrite(WHEEL_IN4, LOW);
  }
  else {
    digitalWrite(WHEEL_IN1, LOW);
    digitalWrite(WHEEL_IN3, LOW);

    digitalWrite(WHEEL_IN2, HIGH);
    digitalWrite(WHEEL_IN4, HIGH);
  }


  analogWrite(WHEEL_PWMA, speed);
  analogWrite(WHEEL_PWMB, speed);
}

void rotate(bool direction, int speed) {

  if(direction) {
    digitalWrite(WHEEL_IN1, HIGH);
    digitalWrite(WHEEL_IN4, HIGH);

    digitalWrite(WHEEL_IN3, LOW);
    digitalWrite(WHEEL_IN2, LOW);

  }
  else {
    digitalWrite(WHEEL_IN1, LOW);
    digitalWrite(WHEEL_IN4, LOW);

    digitalWrite(WHEEL_IN3, HIGH);
    digitalWrite(WHEEL_IN2, HIGH);
  }

  analogWrite(WHEEL_PWMA, speed);
  analogWrite(WHEEL_PWMB, speed);
}

void stop() {
    digitalWrite(WHEEL_IN1, LOW);
    digitalWrite(WHEEL_IN2, LOW);
    digitalWrite(WHEEL_IN3, LOW);
    digitalWrite(WHEEL_IN4, LOW);
    digitalWrite(WHEEL_PWMA, LOW);
    digitalWrite(WHEEL_PWMB, LOW);
    analogWrite(TOP_SIGNAL, 0);
}

//Causes the left arm to spin in a given direction.
void rotateLeftArm(bool direction, int speed) {

  if(direction) {
    digitalWrite(ARM_IN1, HIGH);
    digitalWrite(ARM_IN2, LOW);

  }
  else {
    digitalWrite(ARM_IN1, LOW);
    digitalWrite(ARM_IN2, HIGH);
  }


  analogWrite(ARM_PWMA, speed);
}

//Stops the left arm from spinning.
void stopLeftArm() {
  digitalWrite(ARM_IN1, LOW);
  digitalWrite(ARM_IN2, LOW);
  digitalWrite(ARM_PWMA, LOW);
}

//Causes the right arm to start spinning in a given direction.
void rotateRightArm(bool direction, int speed) {

  if(direction) {
    digitalWrite(ARM_IN3, HIGH);
    digitalWrite(ARM_IN4, LOW);

  }
  else {
    digitalWrite(ARM_IN3, LOW);
    digitalWrite(ARM_IN4, HIGH);
  }


  analogWrite(ARM_PWMB, speed);
}

//Stops the right arm.
void stopRightArm() {
  digitalWrite(ARM_IN3, LOW);
  digitalWrite(ARM_IN4, LOW);
  digitalWrite(ARM_PWMB, LOW);
}

//Turn on the disco ball.
void turnOnTop() {
  analogWrite(TOP_SIGNAL, 190);
}

//Turn off the disco ball.
void turnOffTop() {
  analogWrite(TOP_SIGNAL, 0);
}

void setLeftEyeColor(int8_t *color) {

  for(int i=0;i<NUM_PIXELS;i++){
    // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
    pixelsLeft.setPixelColor(i, pixelsLeft.Color(color[0],color[1],color[2]));
  }
  pixelsLeft.show();
}

void setRightEyeColor(int8_t *color) {

  for(int i=0;i<NUM_PIXELS;i++){
    // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
    pixelsRight.setPixelColor(i, pixelsRight.Color(color[0],color[1],color[2]));
  }
  pixelsRight.show();
}


void handleCommand(RobotCommand command){
  switch(command.type) {
    case RobotOrderType.HELLO:
    {
      // If the cards haven't say hello, check the connection
      if(!is_connected)
      {
        is_connected = true;
        write_order(RobotOrderResponse.THERE);
      }
      else
      {
        // If we are already connected do not send "hello" to avoid infinite loop
        write_order(RobotOrderResponse.ALREADY_CONNECTED);
      }
	  break;
    }
    case RobotOrderType.ALREADY_CONNECTED:
    {
      is_connected = true;
	  break;
    }
    case RobotOrderType.DISCO_BALL:
    {
      turnOnTop();
      break;
    }
    case RobotOrderType.LEFT_EYE_COLOR:
    {
	  int8_t leftEyeColor[3] = {command.data1, command.data2, command.data3};
      setLeftEyeColor(leftEyeColor);
      break;
    }
    case RobotOrderType.RIGHT_EYE_COLOR:
    {
	  int8_t rightEyeColor[3] = {command.data1, command.data2, command.data3};
      setRightEyeColor(rightEyeColor);
      break;
    }
    case RobotOrderType.LEFT_ARM:
    {
      rotateLeftArm(true, 100);
      break;
    }
    case RobotOrderType.RIGHT_ARM:
    {
      rotateRightArm(true, 100);
      break;
    }
    case RobotOrderType.FORWARD:
    {
      move(true, 100);
      break;
    }
    case RobotOrderType.BACKWARD:
    {
      move(false, 100);
      break;
    }
    case RobotOrderType.TURN_LEFT:
    {
      rotate(true, 100);
      break;
    }
    case RobotOrderType.TURN_RIGHT:
    {
      rotate(false, 100);
      break;
    }
    case RobotOrderType.STOP:
    {
      stop();
      stopLeftArm();
      stopRightArm();
      turnOffTop();
	  int8_t offColor[3] = {0, 0, 0};
      setLeftEyeColor(offColor);
	  setRightEyeColor(offColor);
      break;
    }
    // Unknown order
    default:
      write_order(RobotOrderResponse.ERROR);
      return;
  }
  write_order(RobotOrderResponse.RECEIVED); // Confirm the reception
}

void setup()
{
  // Init Serial
  Serial.begin(SERIAL_BAUD);
  pinMode(WHEEL_IN1, OUTPUT);
  pinMode(WHEEL_IN2, OUTPUT);
  pinMode(WHEEL_IN3, OUTPUT);
  pinMode(WHEEL_IN4, OUTPUT);
  pinMode(WHEEL_PWMA, OUTPUT);
  pinMode(WHEEL_PWMB, OUTPUT);
  pinMode(ARM_IN1, OUTPUT);
  pinMode(ARM_IN2, OUTPUT);
  pinMode(ARM_IN3, OUTPUT);
  pinMode(ARM_IN4, OUTPUT);
  pinMode(LEFT_EYE, OUTPUT);
  pinMode(RIGHT_EYE, OUTPUT);
  pinMode(TOP_POWER, OUTPUT);
  pinMode(ARM_PWMA, OUTPUT);
  pinMode(ARM_PWMB, OUTPUT);

  digitalWrite(WHEEL_IN1, LOW);
  digitalWrite(WHEEL_IN2, LOW);
  digitalWrite(WHEEL_IN3, LOW);
  digitalWrite(WHEEL_IN4, LOW);
  analogWrite(WHEEL_PWMA, 0);
  analogWrite(WHEEL_PWMB, 0);
  digitalWrite(ARM_IN1, LOW);
  digitalWrite(ARM_IN2, LOW);
  digitalWrite(ARM_IN3, LOW);
  digitalWrite(ARM_IN4, LOW);
  analogWrite(ARM_PWMA, 0);
  analogWrite(ARM_PWMB, 0);

  digitalWrite(TOP_POWER, HIGH); //Using A0 as a 5v ref
  analogWrite(TOP_SIGNAL, 0);

  pixelsLeft.begin();
  pixelsLeft.setBrightness(10);

  pixelsRight.begin();
  pixelsRight.setBrightness(10);
}

void loop(){
  //Get a robot command from the serial
  //Process the command (local).
  RobotOrder command;
  command = get_command_from_serial();
  handleCommand(command);
}