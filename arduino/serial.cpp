#include "order.h"

//Blocking method to get the next command from master.
RobotOrder get_command_from_serial(){
  RobotOrderType current_order = read_order();
  RobotOrder return_value;
  while(current_order == RobotOrderType.NOOP) {
    current_order = read_order();
  }
  return_value.type = current_order;
  if (current_order == RobotOrderType.LEFT_EYE_COLOR || current_order == RobotOrderType.RIGHT_EYE_COLOR){
    int8_t color[3];
	read_color(color);
	return_value.data1 = color[0];
	return_value.data2 = color[1];
	return_value.data3 = color[2];
  }  
  return return_value;  
}

RobotOrderType read_order(){
  wait_for_bytes(1,1000); // Wait for 1 byte with a timeout of 1s.
  if(Serial.available() > 0)
    return (RobotOrderType) Serial.read();
  else
	return RobotOrderType.NOOP;
}

void wait_for_bytes(int num_bytes, unsigned long timeout){
  unsigned long startTime = millis();
  //Wait for incoming bytes or exit if timeout
  while ((Serial.available() < num_bytes) && (millis() - startTime < timeout)){}
}

void read_color(int8_t* buffer) {
  wait_for_bytes(3, 100); // Wait for 2 bytes with a timeout of 100 ms
  read_signed_bytes(buffer, 3);
}

//This is used when I want to get multiple bytes from the stream.
void read_signed_bytes(int8_t* buffer, size_t n){
  size_t i = 0;
  int c;
  while (i < n){
    c = Serial.read();
	if (c < 0) break;
    *buffer++ = (int8_t) c;
    i++;
  }
}

//We'll eventually use this to read desired motor speed.
int8_t read_i8(){
  wait_for_bytes(1, 100); // Wait for 1 byte with a timeout of 100 ms
  return (int8_t) Serial.read();
}

void write_order(RobotOrderResponse response){
  Serial.write((int8_t)response);
}