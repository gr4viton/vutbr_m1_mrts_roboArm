/***************
@project  roboArm
@filename readFile.cpp
@author   xdavid10, xslizj00 @ FEEC-VUTBR 
@date     2013_12_07
@brief    file containing definitions of functions per reading a file
***************/

#include "roboArm.h"


/****************************************************************************
@function   READ_patialConfigurationFromFile
@brief      	parsing out the parameters for individual phases from the string [str]
@param[in]  
@param[out] 
@return     	on Success	= FLAWLESS_EXECUTION
			on Error		= error_sum of ERRORS defined in returnCodeDefines.h
***************/
int READ_patialConfigurationFromFile(C_roboticManipulator* a_ROB ){
	READ_file();
	//return(a_ROB->CONVERT_angle2int_zero(i);
	return(1);
}

/****************************************************************************
@function   READ_file
@brief      routine for reading out control txt file into string [str]
@param[in]  
@param[out] 
@return     	on Success	= FLAWLESS_EXECUTION
			on Error		= error_sum of ERRORS defined in returnCodeDefines.h
***************/
int READ_file(void){
	int error_sum = 0;
	HANDLE hFile = NULL;
	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// CreateFile - for read handle 
#ifdef DEBUG_PRINT_READ_FUNCTIONS
	RtPrintf("Try to CreateFile.\n");
#endif
	// CONST CHAR * = LPCSTR 
	const char file_path[] = "D:\\EDUC\\m1\\R_MRTS\\float.txt";
	// try lenght of string file_path 
	for(int i=0; file_path[i] != '\0'; i++)
	{
		if( i>=MAX_PATH ) return(ERROR_FILE_PATH_STRING_TOO_LONG);
	}
	hFile = CreateFile(file_path, GENERIC_READ, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE) { // Failed CreateFile
		error_sum = ERROR_CREATEFILE_FAIL;
		//LogMessage()
		// ifdef
		RtPrintf("Function CreateFile failed with 0x%04x - INVALID_HANDLE_VALUE\n", GetLastError());
		return(ERROR_CREATEFILE_FAIL);
	}
#ifdef DEBUG_PRINT_READ_FUNCTIONS
	RtPrintf("CreateFile completed successfully.\n");
#endif
	
	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// SetFilePointer
	/*
	DWORD file_end_byte = 0;
	DWORD file_begin_byte = 0;
	//SET_byte_pointer
#ifdef DEBUG_PRINT_READ_FUNCTIONS
	RtPrintf("Try to SetFilePointer to FILE_END:\n");
#endif
	
	file_end_byte = SetFilePointer(hFile, 0, NULL, FILE_END);
	if (file_end_byte == INVALID_SET_FILE_POINTER) {// Failed to SetFilePointer
		RtPrintf("Function SetFilePointer failed with 0x%04x\n", GetLastError());		
		CLOSE_handleAndReturn(hFile, SUMFLOATS_ERROR_SETFILEPOINTER_FAIL);
	}

	MOVE_pointer(hFile, 0, &file_end_byte, FILE_END);
#ifdef DEBUG_PRINT_READ_FUNCTIONS
	RtPrintf("Try to SetFilePointer to FILE_BEGIN:\n");
#endif
	MOVE_pointer(hFile, 0, &file_begin_byte, FILE_BEGIN);
	*/

	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// ReadFile

	//DWORD file_current_byte = file_begin_byte;
	//unsigned long max = 0;
	//max - 1;

	//DWORD bytes2get = FILE_MAX_CHARS;
	DWORD bytes2get = 100;
	
	//DWORD digit[NUM_OF_DIGITS]; 
	
	//int str_len = 1;
	
#ifdef DEBUG_PRINT_READ_FUNCTIONS
//	RtPrintf("Try to READ_chunk [%lu bytes] from file.\n",bytes2get);
	RtPrintf("Try to ReadFile. Read whole file: [%s]\n", CONTROL_FILE_PATH);
#endif
	DWORD bytes_got;
	// BOOL ReadFile(HANDLE hFile, LPVOID lpBuffer, DWORD nbytes2get, LPDWORD lpbytes_got, LPOVERLAPPED lpOverlapped);
	if (	 FALSE == ReadFile( hFile, (LPVOID) (str), bytes2get, &bytes_got, NULL) ) 
	{ // Failed to ReadFile
		RtPrintf("ERROR:\tFunction ReadFile failed with 0x%04x - returned FALSE\n", GetLastError());
		return(CLOSE_handleAndReturn(hFile, ERROR_READFILE_FAIL));
	}
	else if( bytes_got == 0){
		// reading beyond EOF
#ifdef DEBUG_PRINT_READ_FUNCTIONS
		RtPrintf("Reading ended = EOF\n");
#endif
	}
#ifdef DEBUG_PRINT_READ_FUNCTIONS
	RtPrintf("bytes_got = %lu\n", bytes_got);	
#endif
	str[bytes_got+1] = 0;
	printf("[FILE_START]\n%s\n[FILE_END]",str);

	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// CloseHandle
	CLOSE_handleAndReturn(hFile, FLAWLESS_EXECUTION);	

	return(FLAWLESS_EXECUTION);
}
