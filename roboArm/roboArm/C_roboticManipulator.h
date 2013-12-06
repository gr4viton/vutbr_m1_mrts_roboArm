/***************
@filename		C_roboticManipulator.h
@author 		xdavid10, xslizj00 @ FEEC-VUTBR 
@date			2013_12_02
@brief			file containing C_roboticManipulator class definition  
				= member function and member variables declarations
***************/

#ifndef C_ROBOTICMANIPULATOR_H
#define C_ROBOTICMANIPULATOR_H

#include "roboArm.h"
//#include <rtapi.h> // LNK2019 problem? nope

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// class C_roboticManipulator
class C_roboticManipulator{
//____________________________________________________
// member variables
private:
	C_servoMotor serv[SUM_SERVOMOTORS];	
//____________________________________________________
// declaration of external defined member functions 
private:		bool IS_in_bounds(int servo_i);	
public:		C_roboticManipulator(int &roboticManipulator_error);
public:		int SET_dutyCycleIntervals(int servo_i, LARGE_INTEGER a_interval_one, LARGE_INTEGER a_interval_zero);
public:		int GET_servoMotor(int a_servo_i, C_servoMotor** servoMotor);
			
public:		void SET_portUchar(PUCHAR a_port_address, UCHAR a_port_data);
public:		void SET_bitUchar(PUCHAR a_port_address, UCHAR a_port_bit);
};

#endif