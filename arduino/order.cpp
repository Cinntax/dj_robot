//Used to determine how many bytes of data to read as arguments into the specified commands.
int get_order_data_bytes(RobotOrderType cmd_type) {
  int retVal = 0;
  switch(cmd_type) {
	case DISCO_BALL:
		retVal = 1;
		break;
	case LEFT_EYE_COLOR:
		retVal = 3;
		break;
	case RIGHT_EYE_COLOR:
		retVal = 3;
		break;
	case LEFT_ARM: 
		retVal = 2;
		break;
	case RIGHT_ARM:
		retVal = 2;
		break;
	case FORWARD:
		retVal = 1;
		break;
	case BACKWARD:
		retVal = 1;
		break;
	case TURN_LEFT:
		retVal= 1;
		break;
	case TURN_RIGHT:
		retVal = 1;
		break;
  }
  
  return retVal;
}