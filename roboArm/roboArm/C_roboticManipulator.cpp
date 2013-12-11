/***********
@project		roboArm
@filename	C_roboticManipulator.cpp
@author		xdavid10, xslizj00 @ FEEC-VUTBR 
@date		2013_12_02
@brief		file containing member function definitions 
			of classes C_roboticManipulator 
***********/

#include "roboArm.h"
//#include "C_roboticManipulator.h"

/****************************************************************************
@function   DEBUG_fillPhases
@class		C_roboticManipulator
@brief      
@param[in]  
@param[out] 
@return     
************/
void C_roboticManipulator::DEBUG_fillPhases(void){
	LARGE_INTEGER intervalZero;
	intervalZero.QuadPart = 1750 * NS100_1US;
	int i_serv = 0;

	phases.push_back(C_spatialConfiguration());
	std::list<C_spatialConfiguration>::iterator actPhase = phases.end();
	actPhase--;
	// add 5 phases to the back
	for(int i_phase = 0; i_phase < 5; i_phase++)
	{ // phases
		for(i_serv = 3; i_serv<SUM_SERVOMOTORS; i_serv++)
		{
			actPhase->SET_servIntervalOne(i_serv, &intervalZero);
		}
		intervalZero.QuadPart += 100;
		actPhase->phaseInterval.QuadPart = 5*NS100_1S;
		// push back next
		phases.push_back(C_spatialConfiguration());
		actPhase++;
	}
	//phases.end();
		//it->
	
}

/****************************************************************************
@function   CONVERT_angle2intervalOne
@class		C_roboticManipulator
@brief      converts the value of angle of bounds [angle_min, angle_max]
			into bounds of servo [a_serv] intervals [min_val, max_val]
				can be rewriten to be faster with external pre-counted variables
@param[in]  
@param[out] (LARGE_INTEGER*) a_intervalOne
@return     error_sum
************/
int C_roboticManipulator::CONVERT_angle2intervalOne(int a_angle, int a_i_serv, LARGE_INTEGER* a_intervalOne)
{
	float relative = 0 ; // from f0.0 to f1.0
	relative = (a_angle - angle_min)/((float)(angle_max - angle_min));
	if(relative>1)
	{
#ifdef CUT_OFF_OUT_OF_BOUNDS_ANGLE_IN_CONTROL_FILE //IF NOT DEFINED
		relative = 1; 
#else
		return(ERROR_ANGLE_OUT_OF_BOUNDS);
#endif
	}
	else if(relative <0)
	{
#ifdef CUT_OFF_OUT_OF_BOUNDS_ANGLE_IN_CONTROL_FILE //IF NOT DEFINED
		relative = 0; 
#else
		return(ERROR_ANGLE_OUT_OF_BOUNDS);
#endif
	}
	a_intervalOne->QuadPart = (DWORD)( 
		serv[a_i_serv].min_val + relative * (serv[a_i_serv].max_val - serv[a_i_serv].min_val)
		);
	return(FLAWLESS_EXECUTION);
}


/****************************************************************************
@function   PUSHFRONT_InitialPhases
@class		C_roboticManipulator
@brief      appends initial phases to the begining of list
@param[in]  
@param[out] 
@return     
************/
DWORD C_roboticManipulator::PUSHFRONT_InitialPhases(void)
{
	//std::list<C_spatialConfiguration> tmp;
	phases.push_front(C_spatialConfiguration()); 
	// initial phase interval
	phases.begin()->phaseInterval.QuadPart = DEFAULT_INITIAL_PHASE_INTERVAL;
	// set initial position
	return(FLAWLESS_EXECUTION);
 }
/****************************************************************************
@function   C_roboticManipulator
@class		C_roboticManipulator
@brief      constructor
@param[in]  
@param[out] 
@return     
************/
C_roboticManipulator::C_roboticManipulator(DWORD &error_sum)
{
	error_sum = FLAWLESS_EXECUTION;

	// init phases
	PUSHFRONT_InitialPhases();
	// init addresses and write zeros to register
	DOport_ByteAddress = (PUCHAR)(baseAddress + DO_High_Byte);
	DOport_lastValue = 1; // to work-around WRITE_portUchar not writing the addres if it is the same
	WRITE_portUchar(DOport_ByteAddress, 0);
	DOport_lastValue = 0; 
	
	PWM_period.QuadPart = DEFAULT_PWM_PERIOD; 
	angle_min = 0;
	angle_max = 1800;

	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	//set constants for individual servos
	int i=0;
	int ret_i;
	int min_intervalZero = 500;
	int max_intervalZero = 2500;
	LARGE_INTEGER val_intervalZero;
	val_intervalZero.QuadPart = PWM_period.QuadPart; // for not writing the one at all on start

	//____________________________________________________
	// servo0
	ret_i = serv[i].SET_constants( i, 0, true, min_intervalZero, max_intervalZero, 5 );
	if(ret_i != i){ 
		error_sum = ERR_CONSTRUCOR_ERROR_OFFSET; 	return;
	} 
	serv[i].SET_intervalZero(val_intervalZero);
	i++;
	//____________________________________________________
	// servo1
	ret_i = serv[i].SET_constants( i, 1, true, min_intervalZero, max_intervalZero, 5 );
	if(ret_i != i){ 
		error_sum = ERR_CONSTRUCOR_ERROR_OFFSET; 	return;
	} 
	serv[i].SET_intervalZero(val_intervalZero);
	i++;
	//____________________________________________________
	// servo2
	ret_i = serv[i].SET_constants( i, 2, true, min_intervalZero, max_intervalZero, 5 );
	if(ret_i != i){ 
		error_sum = ERR_CONSTRUCOR_ERROR_OFFSET; 	return;
	} 
	serv[i].SET_intervalZero(val_intervalZero);
	i++;
	//____________________________________________________
	// servo3
	ret_i = serv[i].SET_constants( i, 3, false, min_intervalZero, max_intervalZero, 5 );
	if(ret_i != i){ 
		error_sum = ERR_CONSTRUCOR_ERROR_OFFSET; 	return;
	} 
	serv[i].SET_intervalZero(val_intervalZero);
	i++;
	//____________________________________________________
	// servo4
	ret_i = serv[i].SET_constants( i, 4, false, min_intervalZero, max_intervalZero, 5 );
	if(ret_i != i){ 
		error_sum = ERR_CONSTRUCOR_ERROR_OFFSET; 	return;
	} 
	serv[i].SET_intervalZero(val_intervalZero);
	i++;
	//____________________________________________________
	// servo5
	ret_i = serv[i].SET_constants( i, 5, false, min_intervalZero, max_intervalZero, 5 );
	if(ret_i != i){ 
		error_sum = ERR_CONSTRUCOR_ERROR_OFFSET; 	return;
	} 
	serv[i].SET_intervalZero(val_intervalZero);
	i++;
	return;
}

/****************************************************************************
@function	IS_in_bounds
@class		C_roboticManipulator
@brief		in bounds of 0 to max_servo_i
@param[in]
@param[out]
@return
***************/
int C_roboticManipulator::IS_in_bounds(int servo_i){
	if(servo_i<0 || servo_i>=SUM_SERVOMOTORS) return(ERR_SERVO_INDEX_OUT_OF_BOUNDS);
	else return(FLAWLESS_EXECUTION);
}


/****************************************************************************
@function	GET_servoMotor
@class		C_roboticManipulator
@brief
@param[in]
@param[out]
@return
***************/
DWORD C_roboticManipulator::GET_servoMotor(int a_servo_i, C_servoMotor** servoMotor)
//DWORD C_roboticManipulator::GET_servoMotor(int a_servo_i, C_servoMotor* servoMotor)
{
	DWORD error_sum = IS_in_bounds(a_servo_i);
	if(error_sum != FLAWLESS_EXECUTION) 
		return(error_sum);
	else 
		*servoMotor = &serv[a_servo_i];
	return(error_sum);
}

/****************************************************************************
@function	SET_portUchar
@class		C_roboticManipulator
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
	if(DOport_lastValue == a_port_data)
	{ // register byte did not change
		printf("Do not have to write byte - port has not changed.\n");
	}
	else
	{ // write new byte
		DOport_lastValue = a_port_data;
#ifndef DEBUGGING_WITHOUT_HW // if NOT defined
		printf("WRITE > address= 0x%08x | data= 0x%08x\n", a_port_address, a_port_data);
		RtWritePortUchar(PUCHAR Port, UCHAR Data);
		return;
#endif
		printf("DEBUG > address= 0x%08x | data= 0x%08x\n", a_port_address, a_port_data);
	}
}

/****************************************************************************
@function	RESET_DOport
@class		C_roboticManipulator
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
@class		C_roboticManipulator
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
	if(DOport_lastValue & 1<<a_port_bit)
	{ // the port byt is already SET - no change
		return;
	}
	else
	{ // write changed register byte
		WRITE_portUchar(DOport_ByteAddress, DOport_lastValue | 1<<a_port_bit);
	}
}



/****************************************************************************
@function	SET_dutyCycleIntervals
@class		C_roboticManipulator
@brief
@param[in]
@param[out]
@return
***************/
/*
int C_roboticManipulator::SET_dutyCycleIntervals(
	int a_servo_i, 
	LARGE_INTEGER a_interval_one, 
	LARGE_INTEGER a_intervalZero)
{
	if(!IS_in_bounds(a_servo_i)) return(ERR_SERVO_INDEX_OUT_OF_BOUNDS);
	serv[a_servo_i].SET_dutyCycleIntervals(a_interval_one,a_intervalZero);
	return(FLAWLESS_EXECUTION);
}*/