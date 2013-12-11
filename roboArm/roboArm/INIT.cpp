/***************
@project  roboArm
@filename INIT.cpp
@author   xdavid10, xslizj00 @ FEEC-VUTBR 
@date     2013_12_02
@brief    file containing INIT function definitions
***************/

#include "roboArm.h"

/****************************************************************************
@function		INIT_HW
@brief			
				initializes all
@retval[out]	FLAWLESS_EXECUTION - on flawless execution
***************/
int INIT_HW(){
	DWORD error_sum = 0;
	//____________________________________________________
	// Load library
	printf("> Try to initialize Library\n");
	error_sum = INIT_Library();
	if(error_sum){
		logMsg->PushMessage("Cannot init library\n", PUSHMSG_SEVERITY_NORMAL);
#ifndef DEBUGGING_WITHOUT_HW //if NOT defined
		return(ERR_INIT_CANNOT_LOAD_LIBRARY);	
#endif
		logMsg->PushMessage(">> Continuing as DEBUGGING_WITHOUT_HW was defined!\n", PUSHMSG_SEVERITY_NORMAL);
	}
	else 
		logMsg->PushMessage(">> Library was opened successfully!\n", PUSHMSG_SEVERITY_NORMAL);
	
	//____________________________________________________
	// Initialize ADC
	printf("> Try to initialize ADC\n");
	INIT_ADC();
	return(FLAWLESS_EXECUTION);
}

/****************************************************************************
@function		INIT_ADC
@brief			
				initializes all
@retval[out]	FLAWLESS_EXECUTION - on flawless execution
***************/
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// INIT_ADC
void INIT_ADC()
{
	UCHAR data;
	// Reset
	data = RtReadPortUchar((PUCHAR)(baseAddress));
	data = data | 0x01;
	RtWritePortUchar((PUCHAR)(baseAddress), data);
	// Set software triggering
	RtWritePortUchar((PUCHAR)(baseAddress+AD_MODE_CONTROLL), 0xf1);
	RtSleep(100);
}


//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
/**
 @name 	INIT_Library
 @brief	function open library for communication with PIO821 card
 @retval	0 - function succeeded
 @retval	1 - loadLibrary error, communication FAILED
 @retval	2 - get adrees error, communication FAILED
*/
/****************************************************************************
@function   INIT_Library
@brief      function open library for communication with PIO821 card
@param[in]  
@param[out] 
@return     (int)
			FLAWLESS_EXECUTION - function succeeded
			ERROR-code - defined in returnCodeDefines.h
************/
int INIT_Library()
{
	char textMsg[LENGTH_OF_BUFFER]; // char array for printing messages

	LPCSTR lpLibFileName		= "dac_dll.rtdll";
	LPCSTR lpProcName		= "GetPIO821BaseAddress";
	FARPROC functionPointer = NULL;

	// ____________________________________________________
	// load Library 
	printf("Try to load Library.\n");
	hLibModule = LoadLibrary(lpLibFileName);
	if(hLibModule == NULL) 
	{
		printf("Error:\tCould not load the library.\n");
		return(ERROR_COULD_NOT_LOAD_DAC_DLL_RTDLL_LIBRARY);
	}
	//____________________________________________________
	// Get function from Rtdll 
	printf("Try to get function pointer.\n");
	functionPointer = GetProcAddress( hLibModule, lpProcName) ;
	if(functionPointer == NULL) 
	{
		printf("Error:\tCould not find address.\n");
		FreeLibrary(hLibModule);
		return(ERROR_COULD_NOT_FIND_PROC_ADDRESS);
	}
	// Call function - to get the address
	baseAddress = (DWORD) functionPointer();

	sprintf_s(textMsg, LENGTH_OF_BUFFER, "baseAddress = %i = hex = %x \n", baseAddress, baseAddress);
	logMsg->PushMessage(textMsg, PUSHMSG_SEVERITY_NORMAL);
	// Free the Library
	printf("Free the Library.\n");
	FreeLibrary(hLibModule);
	return(FLAWLESS_EXECUTION);
}