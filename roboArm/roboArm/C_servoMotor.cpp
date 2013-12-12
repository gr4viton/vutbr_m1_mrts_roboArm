/***************
@project  roboArm
@filename C_servoMotor.cpp
@author   xdavid10, xslizj00 @ FEEC-VUTBR 
@date     2013_12_02
@brief    file containing member function definitions of class C_servoMotor
***************/

#include "roboArm.h"
//#include "C_servoMotor.h"

/****************************************************************************
@function	C_servoMotor
@class		C_servoMotor
@brief		constructor
			Servo motors have their position regulated by pulses of different width.
@param[in]		
***************/
C_servoMotor::C_servoMotor(void)
	:min_val(0),max_val(255),mean_vals(1),servo_index(-1) //, ...
{return;}

/****************************************************************************
@function	SET_constants
@class		C_servoMotor
@brief			
			Servo motors have their position regulated by pulses of different width.
@param[in]		
***************/
int C_servoMotor::SET_constants(
	int a_servo_index,
	UCHAR a_servoMotorDigit,
	bool a_FDBACK,
	DWORD a_min_val, DWORD a_max_val, DWORD a_mean_vals
	)
{
	// warning: should not change constants -> it resets intervals
	intervalZero.QuadPart = 0;
	actualADvalue = 0;
	// stop the PWM function and timers!!

	FDBACK = a_FDBACK;
	min_val = a_min_val;
	max_val = a_max_val;
	mean_vals = a_mean_vals;
	servo_index = a_servo_index;
	servoMotorDigit = a_servoMotorDigit;
	return(servo_index);
}

/****************************************************************************
@function	SET_intervalZero
@class		C_servoMotor
@brief		
@param[in]	
@param[out]	
@return		
***************/
void C_servoMotor::SET_intervalZero(LARGE_INTEGER a_intervalZero)
{
	intervalZero.QuadPart = a_intervalZero.QuadPart;
	return;
}


	
/****************************************************************************
@function	~C_servoMotor
@class		C_servoMotor
@brief		destructor
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








/****************************************************************************
@function	SET_dutyCycleIntervals
@class		C_servoMotor
@brief		
			Servo motors have their position regulated by pulses of different width.
@param[in]		
***************/
/*
int C_servoMotor::SET_dutyCycleIntervals(LARGE_INTEGER a_interval_one, LARGE_INTEGER a_intervalZero)
{
	interval_one.QuadPart = a_interval_one.QuadPart;
	intervalZero.QuadPart = a_intervalZero.QuadPart;
	return(FLAWLESS_EXECUTION);
}
	*/