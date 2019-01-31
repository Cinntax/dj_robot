#include "order.h"

void wait_for_bytes(int num_bytes, unsigned long timeout){
  unsigned long startTime = millis();
  //Wait for incoming bytes or exit if timeout
  while ((Serial.available() < num_bytes) && (millis() - startTime < timeout)){}
}

RobotOrderType read_order(){
  wait_for_bytes(1,1000); // Wait for 1 byte with a timeout of 1s.
  if(Serial.available() > 0)
    return (RobotOrderType) Serial.read();
  else
	return NOOP;
}

//This is used when I want to get multiple bytes from the stream.
void read_unsigned_bytes(uint8_t* buffer, size_t n){
  size_t i = 0;
  int c;
  while (i < n){
    c = Serial.read();
	if (c < 0) break;
    *buffer++ = (uint8_t) c;
    i++;
  }
}

void read_data(uint8_t* buffer, int num_bytes) {
  wait_for_bytes(num_bytes, 100); // Wait for 2 bytes with a timeout of 100 ms
  read_unsigned_bytes(buffer, num_bytes);
}

//Blocking method to get the next command from master.
RobotOrder get_command_from_serial(){
  RobotOrderType current_order = read_order();
  RobotOrder return_value;
  int data_bytes;
  while(current_order == NOOP) {
    current_order = read_order();
  }
  return_value.type = current_order;
  
  data_bytes = get_order_data_bytes(current_order);
  uint8_t data[3]; //3 bytes is max right now.
  read_data(data, data_bytes);
  return_value.data1 = data[0];
  return_value.data2 = data[1];
  return_value.data3 = data[2];
	
  return return_value;  
}

void write_order(RobotOrderResponse response){
  Serial.write((uint8_t)response);
}