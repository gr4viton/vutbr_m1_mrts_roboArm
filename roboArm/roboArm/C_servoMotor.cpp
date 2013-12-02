/***************
* @filename		C_servoMotor.cpp
* @author 		xdavid10, xslizj00 @ FEEC-VUTBR 
* @date			2013_12_02
* @brief			file containing member function definitions of class C_servoMotor
***************/

#include "roboArm.h"
//#include "C_servoMotor.h"

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// constructor
C_servoMotor::C_servoMotor(void)
	:min_val(0),max_val(255),mean_vals(1),servo_index(-1) //, ...
{return;}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// SET
int C_servoMotor::SET_constants(
	int a_servo_index,
	bool a_addressHighByte,
	UCHAR servoMotorBit,
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


	if(addressHighByte == SERVOMOTOR_ADDRESS_HIGHBYTE)
		servoMotorByteAddress = (PUCHAR)(baseAddress + DO_High_Byte);
	else
		servoMotorByteAddress = (PUCHAR)(baseAddress + DO_Low_Byte);

	return(servo_index);
}
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//
int C_servoMotor::SET_dutyCycleIntervals(LARGE_INTEGER a_interval_one, LARGE_INTEGER a_interval_zero)
{
	interval_one.QuadPart = a_interval_one.QuadPart;
	interval_zero.QuadPart = a_interval_zero.QuadPart;
	return(FLAWLESS_EXECUTION);
}
	
// ____________________________________________________
// GET_ADC..
// ____________________________________________________
// PWM - periodic functions
int C_servoMotor::PWM_dutyCycle() 
{
	// writing to a critical section should be treated wisely ! <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	// is the register critical section? I think yess
	// mutex for individual bytes / bites of shadow-register
	// and one core thread reading shadow-register and writing it to the true byteAdress 
	// --- possibly event driven ---
	RtWritePortUchar(servoMotorByteAddress, 1<<servoMotorDigit);
	RtSleepFt(&interval_one);
	// You cannot write zeros everywhere
	RtWritePortUchar((PUCHAR)(baseAddress+DO_Low_Byte), 0x00);
	RtSleepFt(&interval_zero);
	return(FLAWLESS_EXECUTION);
}
