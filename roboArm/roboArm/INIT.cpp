/***************
@project  roboArm
@filename INIT.cpp
@author   xdavid10, xslizj00 @ FEEC-VUTBR 
@date     2013_12_02
@brief    file containing INIT function definitions
***************/

#include "roboArm.h"

/****************************************************************************
@function		INIT_All
@brief			
				initializes all
@retval[out]	FLAWLESS_EXECUTION - on flawless execution
***************/
int INIT_All(){
	int ret = 0;

	// Init logMsg
	logMsg = new C_LogMessageA();

	// Load library
	ret = INIT_Library();
	if(ret){
		//printf("Cannot init library\n");	
		logMsg->PushMessage("Cannot init library", SEVERITY_MAX - 1);
#ifndef DEBUGGING_WITHOUT_HW //if NOT defined
		return(ERR_INIT_CANNOT_LOAD_LIBRARY);	
#endif
		//printf("Continuing as DEBUGGING_WITHOUT_HW was defined!\n");
		logMsg->PushMessage("Continuing as DEBUGGING_WITHOUT_HW was defined!", SEVERITY_MAX - 1);
	}
	else 
		//printf("Library was opened successfully :)\n");
		logMsg->PushMessage("Library was opened successfully :)", SEVERITY_MAX - 1);
	
	//____________________________________________________
	// Initialize ADC
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
int INIT_Library()
{
	// char array for printing messages
	char textMsg[LENGTH_OF_BUFFER];
//typedef __nullterminated CONST CHAR *LPCSTR, *PCSTR;
//typedef __nullterminated CONST WCHAR *LPCWSTR, *PCWSTR;
	LPCSTR lpLibFileName = "dac_dll.rtdll";
	LPCSTR lpProcName = "GetPIO821BaseAddress";
	FARPROC functionPointer = NULL;
	// load Library 
	hLibModule = LoadLibrary(lpLibFileName);
	// check if loadLibrary returned correctly 
	if(hLibModule== NULL) {
		/* ERROR */
		printf("Error:\tCould not load the library.\n");
		return 1;
	}
	// Get function from Rtdll 
	functionPointer = GetProcAddress( hLibModule, lpProcName) ;
	// check if function was found 
	if(functionPointer == NULL) 
	{	// ERROR 
		printf("Error:\tCould not find address.\n");
		FreeLibrary(hLibModule);
		return 2;
	}
	// Call function
	baseAddress = (DWORD) functionPointer();
	//printf("base = %i = hex = %x \n", baseAddress, baseAddress);
	sprintf_s(textMsg, LENGTH_OF_BUFFER, "base = %i = hex = %x \n", baseAddress, baseAddress);
	logMsg->PushMessage(textMsg, SEVERITY_MAX - 1);
	// Free Library 
	FreeLibrary(hLibModule);
	return 0;
}