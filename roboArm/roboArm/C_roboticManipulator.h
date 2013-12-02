/***************
* @filename		C_roboticManipulator.h
* @author 		xdavid10, xslizj00 @ FEEC-VUTBR 
* @date			2013_12_02
* @brief			file containing C_roboticManipulator class definition  
				= member function and member variables declarations
***************/

#ifndef C_ROBOTICMANIPULATOR_H
#define C_ROBOTICMANIPULATOR_H

#include "roboArm.h"

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// class C_roboticManipulator
class C_roboticManipulator{
//____________________________________________________
// member variables
private:
	static const int max_servo_i = 5;
	C_servoMotor serv[max_servo_i+1];	
//____________________________________________________
// member functions declarations
private:		bool IS_in_bounds(int servo_i);	
public:		C_roboticManipulator(int &roboticManipulator_error);
public:		int SET_dutyCycleIntervals(int servo_i, LARGE_INTEGER a_interval_one, LARGE_INTEGER a_interval_zero);
public:		int GET_servoMotor(int a_servo_i, C_servoMotor* servoMotor);
};

#endif