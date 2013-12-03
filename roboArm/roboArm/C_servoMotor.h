/***************
* @filename		C_servoMotor.h
* @author 		xdavid10, xslizj00 @ FEEC-VUTBR 
* @date			2013_12_02
* @brief			file containing C_servoMotor class definition  
				= member function and member variables declarations
***************/

#ifndef C_SERVOMOTOR_H
#define C_SERVOMOTOR_H

#include "roboArm.h"

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// class C_servoMotor
class C_servoMotor{
//____________________________________________________
// member variables
private:	
	int servo_index;
	//____________________________________________________
	// feedback
	DWORD min_val;
	DWORD max_val; 
	DWORD mean_vals; // mean of how many feedback measurements 
	bool FDBACK;
	//____________________________________________________
	// timers
	HANDLE hTimer; // handle of a timer which is connected to this servoMotor
	int periodic_input; // parameter of timer periodic execution function
	//____________________________________________________
	// intervals
	LARGE_INTEGER interval_one; // will depend on duty cycle
	LARGE_INTEGER interval_zero; // will depend on periodic time interval and duty cycle
	// LARGE_INTEGER duty_cycle
	
	//____________________________________________________
	// addresses
	bool addressHighByte; // if true set address for HighByte, otherwise LowByte
	PUCHAR servoMotorByteAddress; // byte in the register for this servoMotor
	UCHAR servoMotorDigit; // in case it is different from servo_index
	// which bit from the byte of the address of DO is for this servoMotor
	
//____________________________________________________
// member functions declarations
public: int CREATE_timer(void);
public:	C_servoMotor(void);
public: ~C_servoMotor(void);
//public:	void RTFCNDCL PWM_dutyCycle(void *a_struct);
public:	int SET_dutyCycleIntervals(LARGE_INTEGER a_interval_one, LARGE_INTEGER a_interval_zero);
public:	int SET_constants(int a_servo_index, bool a_addressHighByte,
		UCHAR servoMotorBit, bool a_FDBACK = false,
		DWORD a_min_val = 0, DWORD a_max_val = 255, DWORD a_mean_vals = 5
		);
};

#endif