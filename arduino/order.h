#ifndef ORDER_H
#define ORDER_H
#include <Arduino.h>
//Contains the definition of the orders that our robot will accept from the main computer.
enum RobotOrderType {
  HELLO = 0,
  DISCO_BALL = 1,
  LEFT_EYE_COLOR = 2,
  RIGHT_EYE_COLOR = 3,
  LEFT_ARM = 4,
  RIGHT_ARM = 5,
  FORWARD = 6,
  BACKWARD = 7,
  TURN_LEFT = 8,
  TURN_RIGHT = 9,
  STOP = 13,
  NOOP = 14
};

enum RobotOrderResponse {
  THERE = 0,
  RECEIVED = 1,
  ERROR = 2,
  ALREADY_CONNECTED = 3
};

int get_order_data_bytes(RobotOrderType cmd_type);

typedef enum RobotOrderType RobotOrderType;
typedef enum RobotOrderResponse RobotOrderResponse;

typedef struct RobotOrders {
  RobotOrderType type;
  uint8_t data1;
  uint8_t data2;
  uint8_t data3;
} RobotOrder;
#endif