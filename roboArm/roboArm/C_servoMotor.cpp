/***************
@filename		C_servoMotor.cpp
@author 		xdavid10, xslizj00 @ FEEC-VUTBR 
@date			2013_12_02
@brief			file containing member function definitions of class C_servoMotor
***************/

#include "roboArm.h"
//#include "C_servoMotor.h"

/****************************************************************************
@function		C_servoMotor
@brief			constructor
				Servo motors have their position regulated by pulses of different width.
@param[in]		
***************/
C_servoMotor::C_servoMotor(void)
	:min_val(0),max_val(255),mean_vals(1),servo_index(-1) //, ...
{return;}

/****************************************************************************
@function		SET_constants
@brief			
				Servo motors have their position regulated by pulses of different width.
@param[in]		
***************/
int C_servoMotor::SET_constants(
	int a_servo_index,
	bool a_addressHighByte,
	UCHAR a_servoMotorDigit,
	bool a_FDBACK,
	DWORD a_min_val, DWORD a_max_val, DWORD a_mean_vals
	)
{
	interval_one.QuadPart = 0;
	interval_zero.QuadPart = 0;
	// stop the PWM function and timers!!


	addressHighByte = a_addressHighByte,
	FDBACK = a_FDBACK;
	min_val = a_min_val;
	max_val = a_max_val;
	mean_vals = a_mean_vals;
	servo_index = a_servo_index;
	servoMotorDigit = a_servoMotorDigit;

	if(addressHighByte == SERVOMOTOR_ADDRESS_HIGHBYTE)
		servoMotorByteAddress = (PUCHAR)(baseAddress + DO_High_Byte);
	else
		servoMotorByteAddress = (PUCHAR)(baseAddress + DO_Low_Byte);

	return(servo_index);
}

/****************************************************************************
@function		SET_dutyCycleIntervals
@brief			
				Servo motors have their position regulated by pulses of different width.
@param[in]		
***************/
int C_servoMotor::SET_dutyCycleIntervals(LARGE_INTEGER a_interval_one, LARGE_INTEGER a_interval_zero)
{
	interval_one.QuadPart = a_interval_one.QuadPart;
	interval_zero.QuadPart = a_interval_zero.QuadPart;
	return(FLAWLESS_EXECUTION);
}
	
	
/****************************************************************************
@function		~C_servoMotor
@brief			destructor
				Servo motors have their position regulated by pulses of different width.
@param[in]		-
***************/
C_servoMotor::~C_servoMotor(void)
{
	if(!RtDeleteTimer(hTimer))
	{
		//log
		return;
	}
	if(!RtCloseHandle(hTimer))
	{
		//log
		return;
	}
	// log
}

// ____________________________________________________
// GET_ADC..