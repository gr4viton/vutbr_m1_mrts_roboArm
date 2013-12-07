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
************/
int	READ_spatialConfigurationFromFile(C_roboticManipulator* a_manip, char* a_filePath){
	int error_sum = 0;
	//____________________________________________________
	// read control file into string
	error_sum = READ_file(a_filePath);
	if(error_sum != FLAWLESS_EXECUTION)
	{
		delete[] Gstr;
		printf("READ_file failed with error_sum %i\n", error_sum);
		return(error_sum);
	}
	//____________________________________________________
	// parse control string into individual phases
	error_sum = PARSE_controlString(&(*a_manip));
	if(error_sum != FLAWLESS_EXECUTION)
	{
		delete[] Gstr;
		printf("READ_file failed with error_sum %i\n", error_sum);
		return(error_sum);
	}
	// AFTER creation of new prvek in array of C_spatialConf you must copy non-changed angles from previous phase
	delete[] Gstr;
	//return(a_ROB->CONVERT_angle2int_zero(i);
	return(FLAWLESS_EXECUTION);
}

/****************************************************************************
@function   PARSE_controlString
@brief      parse control string into individual phases
			stored in linear list in [ROB] instance of C_roboticManipulator
@param[in]  
@param[out] 
@return     
************/
int	PARSE_controlString(C_roboticManipulator* a_manip){
	C_roboticManipulator* ROB = (C_roboticManipulator*)a_manip;
	ROB->RESET_DOport();
	ROB = NULL;
	return(FLAWLESS_EXECUTION);
}


/****************************************************************************
@function   READ_file
@brief      routine for reading out control txt file into string [str]
@param[in]  (char*)a_filePath
@param[out] 
@return     	on Success	= FLAWLESS_EXECUTION
			on Error		= error_sum of ERRORS defined in returnCodeDefines.h
***************/
int READ_file(char* a_filePath){
	int error_sum = 0;
	HANDLE hFile = NULL;
	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// CreateFile - for read handle 
#ifdef DEBUG_PRINT_READ_FUNCTIONS
	RtPrintf("Try to CreateFile.\n");
#endif
	// CONST CHAR * = LPCSTR 
	//char file_path[] = "D:\\EDUC\\m1\\R_MRTS\\float.txt";

	hFile = CreateFile(a_filePath, GENERIC_READ, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE) { // Failed CreateFile
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
	DWORD file_end_byte = 0;
#ifdef DEBUG_PRINT_READ_FUNCTIONS
	RtPrintf("Try to SetFilePointer to FILE_END:\n");
#endif
	error_sum = MOVE_pointerOrReturn(hFile, 0, &file_end_byte, FILE_END);
	if(error_sum != FLAWLESS_EXECUTION)	return(CLOSE_handleAndReturn(hFile, error_sum));
	
	DWORD file_begin_byte = 0;
#ifdef DEBUG_PRINT_READ_FUNCTIONS
	RtPrintf("Try to SetFilePointer to FILE_BEGIN:\n");
#endif
	error_sum = MOVE_pointerOrReturn(hFile, 0, &file_begin_byte, FILE_BEGIN);
	if(error_sum != FLAWLESS_EXECUTION)	return(CLOSE_handleAndReturn(hFile, error_sum));

	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// ReadFile

	//DWORD file_current_byte = file_begin_byte;
	//unsigned long max = 0;
	//max - 1;

	//DWORD bytes2get = FILE_MAX_CHARS;
	DWORD bytes2get = file_end_byte;
	try	{
		Gstr = new char[file_end_byte+1];
	}
	catch (std::exception & e) {
		printf("Allocation of char array in READ_file failed with exception:\n%s\n", e.what());
		return(CLOSE_handleAndReturn(hFile, ERROR_BAD_DYNAMIC_ALLOCATION));
	}

#ifdef DEBUG_PRINT_READ_FUNCTIONS
//	RtPrintf("Try to READ_chunk [%lu bytes] from file.\n",bytes2get);
	RtPrintf("Try to ReadFile. Read whole file [%lu bytes] from [%s], \n", bytes2get, a_filePath);
#endif
	DWORD bytes_got;
	// BOOL ReadFile(HANDLE hFile, LPVOID lpBuffer, DWORD nbytes2get, LPDWORD lpbytes_got, LPOVERLAPPED lpOverlapped);
	if (	 FALSE == ReadFile( hFile, (LPVOID) (Gstr), bytes2get, &bytes_got, NULL) ) 
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
	Gstr[bytes_got+1] = 0;
	printf("[FILE_START]\n%s\n[FILE_END]",Gstr);

	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// CloseHandle
	CLOSE_handleAndReturn(hFile, FLAWLESS_EXECUTION);	

	return(FLAWLESS_EXECUTION);
}



/****************************************************************************
@function   CLOSE_handleAndReturn
@brief      
@param[in]  
@param[out] 
@return     
************/
int CLOSE_handleAndReturn(HANDLE handle, int error_sum)
{
#ifdef DEBUG
	RtPrintf("Try to CloseHandle.\n");
#endif
	if( CloseHandle(handle) == 0 )
	{
		RtPrintf("Function CloseHandle failed with 0x%04x\n", GetLastError());
		return(error_sum + ERROR_CLOSEHANDLE_FAIL);
	}
	else 
	{
		printf("Successfully closed handle\n");
		if(error_sum != 0)
			return(error_sum);
		else 
			return(FLAWLESS_EXECUTION);
	}
}

/****************************************************************************
@function   MOVE_pointer
@brief      function moves the pointer in handled file 
			to distance relative to current position/ absolute to start 
			as defined by [MoveMethod]
@param[in]  
@param[out] 
@return     error_sum
************/
int MOVE_pointerOrReturn(HANDLE hFile, LONG distance2move, DWORD* file_current_byte, DWORD MoveMethod=FILE_CURRENT)
{
#ifdef DEBUG_PRINT_READ_FUNCTIONS
	RtPrintf("Try to SetFilePointer.\n");
#endif
	*file_current_byte = SetFilePointer(hFile, distance2move, NULL, MoveMethod);
	if (*file_current_byte == INVALID_SET_FILE_POINTER) 
	{ // Failed to SetFilePointer
		RtPrintf("Function SetFilePointer failed with 0x%04x\n", GetLastError());		
		return(CLOSE_handleAndReturn(hFile, ERROR_SETFILEPOINTER_FAIL));
	}
#ifdef DEBUG_PRINT_READ_FUNCTIONS
	RtPrintf("file_current_byte = %lu\n",*file_current_byte);
#endif
	return(FLAWLESS_EXECUTION);
}