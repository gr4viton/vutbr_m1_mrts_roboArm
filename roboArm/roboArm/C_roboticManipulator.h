/***************
@project  roboArm
@filename C_roboticManipulator.h
@author   xdavid10, xslizj00 @ FEEC-VUTBR 
@date     2013_12_02
@brief    file containing C_roboticManipulator & C_spatialConfiguration class definitions  
			= member functions and member variables declarations
***************/

#ifndef C_ROBOTICMANIPULATOR_H
#define C_ROBOTICMANIPULATOR_H

#include "roboArm.h"
//#include <rtapi.h> // LNK2019 problem? nope

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
/****************************************************************************
@class		C_spatialConfiguration
@brief      
@param[in]  
@param[out] 
@return     
***********/
class C_spatialConfiguration{
//____________________________________________________
// member variables
public:
	LARGE_INTEGER phaseInterval;
	LARGE_INTEGER servIntervalZero[SUM_SERVOMOTORS]; // interval_zeros for each servo in this phase
	bool servIntervalZeroChanged[SUM_SERVOMOTORS]; // if it was changed after constructor = true
//____________________________________________________
// declaration of external defined member functions 
public:	C_spatialConfiguration(void);
public:	C_spatialConfiguration(LARGE_INTEGER* a_phaseInterval, LARGE_INTEGER* a_servIntervalZero);
public: ~C_spatialConfiguration(void);
public: void SET_servIntervalZero(int a_i_serv, LARGE_INTEGER *a_servIntervalZero);
};


//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
/****************************************************************************
@class		C_roboticManipulator
@brief      
@param[in]  
@param[out] 
@return     
***************/
class C_roboticManipulator{
//____________________________________________________
// member variables
private:
	C_servoMotor serv[SUM_SERVOMOTORS];	
	PUCHAR DOport_ByteAddress; // address of DO port in register to whom are servoMotors connected to
	UCHAR DOport_lastValue;
	LARGE_INTEGER PWM_period;
	int angle_min;
	int angle_max;
public:
	std::list<C_spatialConfiguration> phases;
//____________________________________________________
// declaration of external defined member functions 
public:		int IS_in_bounds(int servo_i);	
public:		C_roboticManipulator(int &roboticManipulator_error);
//public:		int SET_dutyCycleIntervals(int servo_i, LARGE_INTEGER a_interval_one, LARGE_INTEGER a_interval_zero);
public:		int GET_servoMotor(int a_servo_i, C_servoMotor** servoMotor);
			
public:		void WRITE_portUchar(PUCHAR a_port_address, UCHAR a_port_data);
public:		void RESET_DOport();
public:		void SET_DOportBitUchar(UCHAR a_port_bit);
public:		int CONVERT_angle2int_zero(int a_angle, int a_i_serv, LARGE_INTEGER* a_interval_zero);
};

#endif