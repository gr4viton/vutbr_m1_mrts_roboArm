/***************
@project  roboArm
@filename C_roboticManipulator.cpp
@author   xdavid10, xslizj00 @ FEEC-VUTBR 
@date     2013_12_02
@brief    file containing member function definitions of class C_roboticManipulator
***************/

#include "roboArm.h"
//#include "C_roboticManipulator.h"
//const int C_roboticManipulator::max_servo_i = SUM_SERVOMOTORS-1;

/****************************************************************************
@function	C_roboticManipulator
@brief		constructor
@param[in]
@param[out]
@return
***************/
C_roboticManipulator::C_roboticManipulator(int &roboticManipulator_error)
{
	roboticManipulator_error = FLAWLESS_EXECUTION;
	
	DOport_ByteAddress = (PUCHAR)(baseAddress + DO_High_Byte);
	WRITE_portUchar(DOport_ByteAddress,0);
	
	//PWM_period.QuadPart = NS100_1S / 100; // 1/100 s = 100 Hz
	PWM_period.QuadPart = NS100_1S / 1; // 1/1 s = 1 Hz

	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	//set constants for individual servos
	int i=0;
	int ret_i;
	//____________________________________________________
	// servo0
	ret_i = serv[i].SET_constants( i, 0,
		true, 1,255, 5 );
	if(ret_i != i){ 
		roboticManipulator_error = ERR_CONSTRUCOR_ERROR_OFFSET; 	return;
	} 
	serv[i].SET_intervalZero(PWM_period);
	i++;
	//____________________________________________________
	// servo1
	ret_i = serv[i].SET_constants( i, 1,
		true, 1,255, 5 );
	if(ret_i != i){ 
		roboticManipulator_error = ERR_CONSTRUCOR_ERROR_OFFSET; 	return;
	} 
	serv[i].SET_intervalZero(PWM_period);
	i++;
	//____________________________________________________
	// servo2
	ret_i = serv[i].SET_constants( i, 2,
		true, 1,255, 5 );
	if(ret_i != i){ 
		roboticManipulator_error = ERR_CONSTRUCOR_ERROR_OFFSET; 	return;
	} 
	serv[i].SET_intervalZero(PWM_period);
	i++;
	//____________________________________________________
	// servo3
	ret_i = serv[i].SET_constants( i, 3
		);
	if(ret_i != i){ 
		roboticManipulator_error = ERR_CONSTRUCOR_ERROR_OFFSET; 	return;
	} 
	serv[i].SET_intervalZero(PWM_period);
	i++;
	//____________________________________________________
	// servo4
	ret_i = serv[i].SET_constants( i, 4
		);
	if(ret_i != i){ 
		roboticManipulator_error = ERR_CONSTRUCOR_ERROR_OFFSET; 	return;
	} 
	serv[i].SET_intervalZero(PWM_period);
	i++;
	//____________________________________________________
	// servo5
	ret_i = serv[i].SET_constants( i, 5
		);
	if(ret_i != i){ 
		roboticManipulator_error = ERR_CONSTRUCOR_ERROR_OFFSET; 	return;
	} 
	serv[i].SET_intervalZero(PWM_period);
	i++;
	//for(int i=0;i<max_servo_i;i++){serv[i] = new C_servoMotor(..)		}
	return;
}

/****************************************************************************
@function	IS_in_bounds
@brief		in bounds of 0 to max_servo_i
@param[in]
@param[out]
@return
***************/
bool C_roboticManipulator::IS_in_bounds(int servo_i){
	if(servo_i<0 || servo_i>=SUM_SERVOMOTORS) return(false);
	else return(true);
}

/****************************************************************************
@function	SET_dutyCycleIntervals
@brief
@param[in]
@param[out]
@return
***************/
int C_roboticManipulator::SET_dutyCycleIntervals(
	int a_servo_i, 
	LARGE_INTEGER a_interval_one, 
	LARGE_INTEGER a_interval_zero)
{
	if(!IS_in_bounds(a_servo_i)) return(ERR_SERVO_INDEX_OUT_OF_BOUNDS);
	serv[a_servo_i].SET_dutyCycleIntervals(a_interval_one,a_interval_zero);
	return(FLAWLESS_EXECUTION);
}

/****************************************************************************
@function	GET_servoMotor
@brief
@param[in]
@param[out]
@return
***************/
int C_roboticManipulator::GET_servoMotor(int a_servo_i, C_servoMotor** servoMotor)
{
	if(!IS_in_bounds(a_servo_i)) return(ERR_SERVO_INDEX_OUT_OF_BOUNDS);
	else *servoMotor = &serv[a_servo_i];
	return(FLAWLESS_EXECUTION);
}

/****************************************************************************
@function	SET_portUchar
@brief		if DEBUGGING_WITHOUT_HW is set 
			-> do not call RTX RtWritePortUchar function
			-> rtPrintf byte instead
			else call RtWritePortUchar with the same input parameters			
@param[in]	
@param[out]	
@return		
***************/
void C_roboticManipulator::WRITE_portUchar(PUCHAR a_port_address, UCHAR a_port_data)
{	
	DOport_lastValue = a_port_data;
#ifndef DEBUGGING_WITHOUT_HW
	RtWritePortUchar(PUCHAR Port, UCHAR Data);
	return;
#endif
	printf("address= 0x%08x | data= 0x%08x\n", a_port_address, a_port_data);
}

/****************************************************************************
@function	RESET_DOport
@brief		Writes zeros into DO port
@param[in]	
@param[out]	
@return		
***************/
void C_roboticManipulator::RESET_DOport()
{	
	WRITE_portUchar(DOport_ByteAddress,(UCHAR)0);
}

/****************************************************************************
@function	SET_bitUchar
@brief		if DEBUGGING_WITHOUT_HW is set 
			-> do not call RTX RtWritePortUchar function
			-> rtPrintf byte instead
			else call RtWritePortUchar with the same input parameters			
@param[in]	
@param[out]	
@return		
***************/
void C_roboticManipulator::SET_DOportBitUchar(UCHAR a_port_bit)
{	
	if(DOport_lastValue && 1<<a_port_bit)
	{ // the port byt is already SET
		return;
	}
	else
	{
		WRITE_portUchar(DOport_ByteAddress, DOport_lastValue || 1<<a_port_bit);
	}
}