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
	:ADC_min(0),ADC_max(255),ADC_meanCount(1),servo_index(-1) //, ...
{return;}

/****************************************************************************
@function	SET_constants
@class		C_servoMotor
@brief			
			Servo motors have their position regulated by pulses of different width.
@param[in]		
***************/
int C_servoMotor::SET_constants(
	int a_servo_index,	UCHAR a_servoMotorDigit,
	LONGLONG a_intervalOne_min, LONGLONG a_intervalOne_max,
	bool a_ADC_feedBack,
	DWORD a_ADC_min, DWORD a_ADC_max, DWORD a_ADC_meanCount
	)
{
	servo_index = a_servo_index;
	servoMotorDigit = a_servoMotorDigit;

	intervalOne_min.QuadPart = a_intervalOne_min;
	intervalOne_max.QuadPart = a_intervalOne_max;
	intervalZero.QuadPart = 0;
	ADC_actual = 0;

	ADC_feedBack = a_ADC_feedBack;	
	ADC_min = a_ADC_min;
	ADC_max = a_ADC_max;
	ADC_meanCount = a_ADC_meanCount;
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
	char textMsg[MAX_MESSAGE_LENGTH];
	sprintf_s(textMsg,"class instance of ServoMechanism[%i], is destructed\n", servo_index);
	logMsg.PushMessage(textMsg, LOG_SEVERITY_NORMAL);
}

/****************************************************************************
@function   GET_ADC_actual
@class		C_servoMotor
@brief      copies the last measured values from ADC to the ADC_actual
@param[in]  
@param[out] 
@return     error_sum
************/
DWORD C_servoMotor::GET_ADC_actual()
{
	// not implemented yet
	ADC_actual = 0;
	return(FLAWLESS_EXECUTION);
}







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