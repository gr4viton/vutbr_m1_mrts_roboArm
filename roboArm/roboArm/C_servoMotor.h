/***************
@project  roboArm
@filename C_servoMotor.h 
@author   xdavid10, xslizj00 @ FEEC-VUTBR 
@date     2013_12_02
@brief    file containing C_servoMotor class definition  
			= member function and member variables declarations
***************/

#ifndef C_SERVOMOTOR_H
#define C_SERVOMOTOR_H

#include "roboArm.h"
//#include <rtapi.h> // LNK2019 problem? nope

/****************************************************************************
@class		C_servoMotor
@brief      
***********/
class C_servoMotor
{
//____________________________________________________
// member variables
public:	
	int servo_index; // maximal SUM_SERVOMOTORS
	bool fixedPositioning;	// if fixed positioning is true -> no linear change of position in PWM
	//____________________________________________________
	// Analog to Digital Conversion
	bool ADC_feedBack;
	DWORD ADC_actual;		// periodicly read AD value 
	DWORD ADC_min;			// minimal read-out from ADC
	DWORD ADC_max;			// maximal read-out from ADC
	DWORD ADC_meanCount;		// compute mean from sum of feedback measurements 
	//____________________________________________________
	// intervals
	LARGE_INTEGER intervalOne_min;
	LARGE_INTEGER intervalOne_max;
	LARGE_INTEGER intervalOne_actual;
	LARGE_INTEGER intervalZero; // will depend on periodic time interval and duty cycle
	//____________________________________________________
	// addresses
	UCHAR servoMotorDigit; // in case it is different from servo_index
	// which bit from the byte of the address of DO is for this servoMotor
	
//____________________________________________________
// declaration of external defined member functions 
public:	C_servoMotor(void);
public: ~C_servoMotor(void);
//public:	int SET_dutyCycleIntervals(LARGE_INTEGER a_interval_one, LARGE_INTEGER a_intervalZero);
public: void SET_intervalZero(LARGE_INTEGER a_intervalZero);

public:	int SET_constants(
	int a_servo_index,	UCHAR a_servoMotorDigit,
	LONGLONG a_intervalOne_min, LONGLONG a_intervalOne_max,
	bool a_ADC_feedBack = false,
	DWORD a_ADC_min = 0, DWORD a_ADC_max = 255, DWORD a_ADC_meanCount = 5
	);
		
public: DWORD GET_ADC_actual();
//____________________________________________________
// other member function definitions
		/*
public: LARGE_INTEGER GET_intervalZero(){return(intervalZero);}
public: LARGE_INTEGER GET_interval_one(){return(interval_one);}
*/
};

#endif