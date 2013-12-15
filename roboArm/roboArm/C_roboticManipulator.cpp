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
	LARGE_INTEGER intervalOne;
	int i_serv = 0;
	
	phases.push_back(C_spatialConfiguration());
	// actPhase should point to the last phase (not end - uninitialized space after last)
	phase_act = phases.end();
	phase_act--;
	// as there is only one phase - prevPhase is the same -> is changed in PWM thread
	phase_prev = phase_act;

	LONGLONG addVal = 100;
	// add 5 phases to the back

	int i_phase_max = 21;
	int i_serv_min = 5;
	
	intervalOne.QuadPart = 500 * NS100_1US; // [us]
	LONGLONG phaseInterval = 100 * NS100_1MS; // [ms]
	
	for(int i_phase = 0; i_phase < i_phase_max ; i_phase++)
	{ // phases
		for(i_serv = i_serv_min; i_serv<SUM_SERVOMOTORS; i_serv++)
		{
			phase_act->SET_serv_intervalOne(i_serv, &intervalOne);
		}
		intervalOne.QuadPart += addVal * NS100_1US;
		phase_act->phaseInterval.QuadPart = phaseInterval;
		// push back next
		if( i_phase < i_phase_max )
		{
			phases.push_back(C_spatialConfiguration());
			phase_act++;
		}
	}
	/*
	//intervalOne.QuadPart = 2500 * NS100_1US;
	addVal = -addVal; 
	for(int i_phase = 0; i_phase < i_phase_max ; i_phase++)
	{ // phases
		for(i_serv = i_serv_min; i_serv<SUM_SERVOMOTORS; i_serv++)
		{
			phase_act->SET_serv_intervalOne(i_serv, &intervalOne);
		}
		intervalOne.QuadPart += addVal * NS100_1US;
		phase_act->phaseInterval.QuadPart = 800*NS100_1MS;
		// push back next
		if( i_phase < i_phase_max )
		{
			phases.push_back(C_spatialConfiguration());
			phase_act++;
		}
	}
	*/
	/*
	// to know individual max and min
	i_serv = 5;
	LONGLONG milliS = 4000;

	intervalOne.QuadPart = 500;
	actPhase->SET_serv_intervalOne(i_serv, &intervalOne);
	actPhase->phaseInterval.QuadPart = milliS*NS100_1MS;
		phases.push_back(C_spatialConfiguration());

	intervalOne.QuadPart = 2500;
	actPhase->SET_serv_intervalOne(i_serv, &intervalOne);
	actPhase->phaseInterval.QuadPart = milliS*NS100_1MS;
		phases.push_back(C_spatialConfiguration());
	i_serv++;
	*/
	
}
/****************************************************************************
@function   SET_NextPhase
@class		C_roboticManipulator
@brief      
@param[in]  
@param[out] 
@return     
************/
DWORD C_roboticManipulator::SET_NextPhase()
{
	char textMsg[MAX_MESSAGE_LENGTH];
	phase_act++;
	if(phase_act == phases.end())
	{
		return(ERROR_CANNOT_SET_NEXTPHASE);
	}
	else
	{
		PWMperiod_sum = 0;
		sprintf_s(textMsg, MAX_MESSAGE_LENGTH, "Continuing with next phase[%i/%i].\n", phase_act->i_phase, phase_act->i_phase_max);
		logMsg.PushMessage(textMsg, LOG_SEVERITY_PWM_PHASE);
	}
	return(FLAWLESS_EXECUTION);
}


/****************************************************************************
@function   LOAD_actualPhase
@class		C_roboticManipulator
@brief      copies the needed data from phase_act into individual C_servoMotor instances 
@param[in]  
@param[out] 
@return     
************/
void C_roboticManipulator::LOAD_actualPhase(void)
{	
	char textMsg[MAX_MESSAGE_LENGTH]; // char array for printing messages
	
	LARGE_INTEGER intervalZero;		// tics for holding one on defined pin
	intervalZero.QuadPart = 0;
	for(int i_serv=0 ; i_serv < SUM_SERVOMOTORS ; i_serv++)
	{
		// count the [zero interval] from [pwm period - one interval]
		intervalZero.QuadPart = PWMperiod_interval.QuadPart - phase_act->serv_intervalOne[i_serv].QuadPart;
		// DEBUG
		//intervalZero.QuadPart = PWMperiod_interval->QuadPart - 1750 * NS100_1US;
		// write it to actual
		serv[i_serv].SET_intervalZero( intervalZero );
		serv[i_serv].fixedPositioning = phase_act->serv_fixedPositioning[i_serv];
	}

	sprintf_s(textMsg, MAX_MESSAGE_LENGTH, "Actual phase[%i/%i] interval = %I64d[ms]\n", 
		phase_act->i_phase, phase_act->i_phase_max, 
		phase_act->phaseInterval.QuadPart / NS100_1MS );
	logMsg.PushMessage(textMsg, LOG_SEVERITY_NORMAL);
}

/****************************************************************************
@function   CONVERT_angle2intervalOne
@class		C_roboticManipulator
@brief      converts the value of angle of bounds [angle_min, angle_max]
			into bounds of servo [a_serv] intervals [ADC_min, ADC_max]
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
		serv[a_i_serv].ADC_min + relative * (serv[a_i_serv].ADC_max - serv[a_i_serv].ADC_min)
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
	for(int i_serv=0; i_serv<SUM_SERVOMOTORS; i_serv++)
	{
		phases.begin()->serv_intervalOne[i_serv].QuadPart = 
			serv[i_serv].ADC_min
			- phases.begin()->serv_intervalOne[i_serv].QuadPart;
	}
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
	// zeros
	PWMperiod_sum = 0;
	PWMperiod_sum_max = 0;

	// init phases - set the default one on the beginning
	PUSHFRONT_InitialPhases();
	
	// init addresses 
	DOport_ByteAddress = (PUCHAR)(baseAddress + DO_High_Byte);

	// write zeros to register
	DOport_lastPeriodValue = 1; // to work-around WRITE_portUchar not writing the addres if it is the same
	WRITE_portUchar(DOport_ByteAddress, 0);
	DOport_lastPeriodValue = 0; 
	
	// the min, max of angle from control-file 
	angle_min = 0;
	angle_max = 1800;

	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	//set constants for individual servos
	int ret_i;
	LONGLONG min_intervalZero = 500;
	LONGLONG max_intervalZero = 2500;

	// debug values -> would be values from empirical measurement for each servo
	DWORD ADC_min = 1700;
	DWORD ADC_max = 2300;

	//____________________________________________________
	// SET constants of servos
	int i=0;		// servo[0]
	ret_i = serv[i].SET_constants( i, S1, min_intervalZero, max_intervalZero, true, ADC_min, ADC_max,5 );
	if(ret_i != i){ 	error_sum = ERR_CONSTRUCOR_ERROR_OFFSET; 	return;	} 	
	i++;			// servo[1]
	ret_i = serv[i].SET_constants( i, S2, min_intervalZero, max_intervalZero, true, ADC_min, ADC_max,5 );
	if(ret_i != i){ 	error_sum = ERR_CONSTRUCOR_ERROR_OFFSET; 	return; } 
	i++; 		// servo[2]
	ret_i = serv[i].SET_constants( i, S3, min_intervalZero, max_intervalZero, true, ADC_min, ADC_max,5 );
	if(ret_i != i){ 	error_sum = ERR_CONSTRUCOR_ERROR_OFFSET; 	return;	} 
	i++;			// servo[3]
	ret_i = serv[i].SET_constants( i, S4, min_intervalZero, max_intervalZero );
	if(ret_i != i){ 	error_sum = ERR_CONSTRUCOR_ERROR_OFFSET; 	return;	} 
	i++;			// servo[4]
	ret_i = serv[i].SET_constants( i, S5, min_intervalZero, max_intervalZero );
	if(ret_i != i){ 	error_sum = ERR_CONSTRUCOR_ERROR_OFFSET; 	return;	} 
	i++;			// servo[5]
	ret_i = serv[i].SET_constants( i, S6, min_intervalZero, max_intervalZero );
	if(ret_i != i){ 	error_sum = ERR_CONSTRUCOR_ERROR_OFFSET; 	return;	} 
	//____________________________________________________
	// end
	error_sum = FLAWLESS_EXECUTION;
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
DWORD C_roboticManipulator::IS_in_bounds(int servo_i){
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
	{
		char textMsg[MAX_MESSAGE_LENGTH]; // char array for printing messages
		sprintf_s(textMsg, MAX_MESSAGE_LENGTH, "Could not get servoMotor[%i] pointer - index out of bounds\n", a_servo_i);
		logMsg.PushMessage(textMsg, SEVERITY_ERROR);
		return(error_sum);
	}
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
	if(DOport_lastPeriodValue == a_port_data)
	{ // register byte did not change
		logMsg.PushMessage("Do not have to write byte - port has not changed.\n", LOG_SEVERITY_PWM_PERIOD);		
	}
	else
	{ // write new byte
		char textMsg[MAX_MESSAGE_LENGTH]; // char array for printing messages
		DOport_lastPeriodValue = a_port_data; // actualize - last from the view of the next period
		DOport_thisPeriodNewValue = DOport_lastPeriodValue; // actualize - this from the view of the next period
#ifndef DEBUGGING_WITHOUT_HW // if NOT defined
		
		sprintf_s(textMsg, MAX_MESSAGE_LENGTH, "WRITE > address= 0x%02x | data= 0x%02x\n", a_port_address, a_port_data);
		RtWritePortUchar(a_port_address, a_port_data);
#else
		
		sprintf_s(textMsg, MAX_MESSAGE_LENGTH, "DEBUG > address= 0x%02x | data= 0x%02x\n", a_port_address, a_port_data);
		// not writing anywhere
#endif
		logMsg.PushMessage(textMsg, LOG_SEVERITY_PWM_PERIOD);
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
	if(DOport_thisPeriodNewValue & 1<<a_port_bit)
	{ // the port bit is already SET - no change
		return;
	}
	else
	{ // write changed bit to DOport_thisPeriodNewValue
		// WRITE_portUchar(DOport_ByteAddress, DOport_lastPeriodValue | 1<<a_port_bit);
		DOport_thisPeriodNewValue = DOport_thisPeriodNewValue | 1<<a_port_bit;
	}
}
/****************************************************************************
@function	WRITE_DOport_thisPeriodNewValue
@class		C_roboticManipulator
@brief
@param[in]
@param[out]
@return
***************/
void C_roboticManipulator::WRITE_DOport_thisPeriodNewValue()
{
	if(DOport_thisPeriodNewValue != DOport_lastPeriodValue)
	{
		WRITE_portUchar(DOport_ByteAddress, DOport_thisPeriodNewValue);
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