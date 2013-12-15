/***************
@project  roboArm
@filename readFile.cpp
@author   xdavid10, xslizj00 @ FEEC-VUTBR 
@date     2013_12_07
@brief    file containing definitions of functions per reading a file
***************/

#include "roboArm.h"

/****************************************************************************
@function   GET_stringLength
@brief      secure strlen() with maximum lenght 
@param[in]  (char*) a_string | string whose lenght we want to know
			(DWORD) a_max_lenght | maximum lenght
@param[out] (int*) error_sum | param to return out error value
@return    	(DWORD)
			- if the lenght of the string is larger than maximum 
				return = 0
				error_sum = ERROR_STRING_LENGHT_LARGER_THAN_TRESHOLD
			- else
				return = number of characters in a_string
				error_sum = FLAWLESS_EXECUTION
************/
DWORD GET_stringLength(char *a_string, DWORD a_max_lenght, DWORD* a_error_sum)
{
	DWORD inStrLen = 0;
	for(inStrLen=0; a_string[inStrLen] != '\0'; inStrLen++){
		if(inStrLen > a_max_lenght) 
		{
			RtPrintf("ERROR - string is too long (max=%u) [%s]\n", a_max_lenght, a_string);
			*a_error_sum = ERROR_STRING_LENGHT_LARGER_THAN_TRESHOLD;
			return(0);
		}
	}
	*a_error_sum = FLAWLESS_EXECUTION;
	return(inStrLen);
}

//DWORD READ_spatialConfigurationFromFile(C_roboticManipulator* a_manip, char* a_filePath)


/****************************************************************************
@function   READ_patialConfigurationFromFile
@brief      	parsing out the parameters for individual phases from the string [str]
@param[in]  C_roboticManipulator* a_ROB | pointer to robotic manipulator class instance
@return     (int)
			on Success	= FLAWLESS_EXECUTION
			on Error		= error_sum of ERRORS defined in returnCodeDefines.h
************/
DWORD READ_spatialConfigurationFromFile(C_roboticManipulator* a_ROB)
{
	// char array for printing messages
	char textMsg[MAX_MESSAGE_LENGTH];
	DWORD error_sum = 0;
	//____________________________________________________
	// read control file into string
	error_sum = READ_file(a_ROB->controlFilePath);
	if(error_sum != FLAWLESS_EXECUTION)
	{
		delete[] G_controlString;
		sprintf_s(textMsg, MAX_MESSAGE_LENGTH, "READ_file failed with error_sum %lu\n", error_sum);
		logMsg.PushMessage(textMsg, LOG_SEVERITY_ERROR);
		return(error_sum);
	}
	logMsg.PushMessage("READ_file completed", LOG_SEVERITY_READING_FILE);
	//____________________________________________________
	// parse control string into individual phases
	logMsg.PushMessage("Try to parse control-file", LOG_SEVERITY_READING_FILE);
	error_sum = PARSE_controlString(&(*a_ROB));
	if(error_sum != FLAWLESS_EXECUTION)
	{
		delete[] G_controlString;
		sprintf_s(textMsg, MAX_MESSAGE_LENGTH, "PARSE_controlString failed with error_sum %lu\n", error_sum);
		logMsg.PushMessage(textMsg, LOG_SEVERITY_ERROR);
		return(error_sum);
	}
	logMsg.PushMessage("PARSE_controlString completed", LOG_SEVERITY_READING_FILE);
	delete[] G_controlString;
	//return(a_ROB->CONVERT_angle2intervalOne(i);*/
	return(FLAWLESS_EXECUTION);
}

/****************************************************************************
@function   PARSE_controlString
@brief      parse control string into individual phases
			stored in linear list in [ROB] instance of C_roboticManipulator
@param[in]  C_roboticManipulator* a_ROB | pointer to robotic manipulator class instance
@return     DWORD error_sum
************/
DWORD PARSE_controlString(C_roboticManipulator* a_ROB)
{
	//char textMsg[MAX_MESSAGE_LENGTH];// char array for printing messages

	C_roboticManipulator* ROB = (C_roboticManipulator*)a_ROB;
	C_spatialConfiguration newPhase;
	//newPhase.phaseInterval.QuadPart = phaseInterval;	

	char* pStr = G_controlString;
	int i_serv = 0;
	int angle = 0;
	bool alreadyChanged = false;
	LARGE_INTEGER largeInteger;
	for(DWORD i=0; pStr[i] != '\0'; ){
		switch(pStr[i])
		{
			case('<'):
			case('>'):
				i_serv = char2num(pStr[i-1]);
				newPhase.serv_fixedPositioning[i_serv] = false;
				alreadyChanged = true;
			case('='):
				if(alreadyChanged == false)
				{
					newPhase.serv_fixedPositioning[i_serv] = true;
				}
				alreadyChanged = false;
				angle = char2num(pStr[i+1])*1000;
				angle += char2num(pStr[i+2])*100;
				angle += char2num(pStr[i+3])*10; 
				angle += char2num(pStr[i+4]); 
				ROB->CONVERT_angle2intervalOne(angle, i_serv, &largeInteger);
				newPhase.serv_intervalOne[i_serv].QuadPart = largeInteger.QuadPart;
				newPhase.serv_intervalOne_changed[i_serv] = true;
				i = i+5;
				break;
			case('W'):
				largeInteger.QuadPart = char2num(pStr[++i]);
				DWORD j = 1;
				for(; (char2num(pStr[i+j]) != ERROR_IS_NOT_NUMBER); j++)
				{
					largeInteger.QuadPart *= 10;
					largeInteger.QuadPart += char2num(pStr[i+j]);
				}	
				i += j+1;
				newPhase.phaseInterval.QuadPart = largeInteger.QuadPart * NS100_1MS;
				ROB->PUSHBACK_newPhase(&newPhase);
				for(i_serv = 0; i_serv<SUM_SERVOMOTORS; i_serv++)
					newPhase.serv_intervalOne_changed[i_serv] = false;
				break;
		}
		i++;
	}
	return(FLAWLESS_EXECUTION);
}

/****************************************************************************
@function   CREATE_fileHandle
@brief      creates a handle to a file and gives it out
@param[in]  char* a_filePath | path to the opened file
@param[out] HANDLE* hFile | pointer to the file handle
@return     DWORD error_sum
************/
DWORD CREATE_fileHandle(HANDLE* a_hFile, char* a_filePath)
{
	char textMsg[MAX_MESSAGE_LENGTH];// char array for printing messages
	*a_hFile = CreateFile(a_filePath, GENERIC_READ, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (*a_hFile == INVALID_HANDLE_VALUE) 
	{ 
		// Failed CreateFile
		sprintf_s(textMsg, MAX_MESSAGE_LENGTH, "Function CreateFile failed with 0x%04x - INVALID_HANDLE_VALUE\n", GetLastError());
		logMsg.PushMessage(textMsg, LOG_SEVERITY_NORMAL);
		return(ERROR_CREATEFILE_FAIL);
	}
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
DWORD READ_file(char* a_filePath)
{
	// char array for printing messages
	char textMsg[MAX_MESSAGE_LENGTH];
	DWORD error_sum = 0;
	HANDLE hFile = NULL;

	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// CreateFile - for read handle 
	logMsg.PushMessage("Try to CreateFile.", LOG_SEVERITY_READING_FILE);

	error_sum = CREATE_fileHandle(&hFile, a_filePath);
	if(error_sum != FLAWLESS_EXECUTION) return(error_sum);
	
	logMsg.PushMessage("CreateFile completed successfully.", LOG_SEVERITY_READING_FILE);
	
	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// SetFilePointer
	DWORD file_end_byte = 0;
	logMsg.PushMessage("Try to SetFilePointer to FILE_END:", LOG_SEVERITY_READING_FILE);
	error_sum = MOVE_pointer(hFile, 0, &file_end_byte, FILE_END);
	if(error_sum != FLAWLESS_EXECUTION)	return(CLOSE_handleAndReturn(hFile, error_sum));
	
	DWORD file_begin_byte = 0;
	logMsg.PushMessage("Try to SetFilePointer to FILE_BEGIN:", LOG_SEVERITY_READING_FILE);
	error_sum = MOVE_pointer(hFile, 0, &file_begin_byte, FILE_BEGIN);
	if(error_sum != FLAWLESS_EXECUTION)	return(CLOSE_handleAndReturn(hFile, error_sum));

	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// ReadFile

	//DWORD bytes2get = CONTROL_FILE_MAX_CHARS;
	DWORD bytes2get = file_end_byte;
	sprintf_s(textMsg, MAX_MESSAGE_LENGTH,
		"Try to allocate char array memory for reading the file [%lu bytes]\n", bytes2get+1);
		logMsg.PushMessage(textMsg, LOG_SEVERITY_READING_FILE);

	try	
	{
		G_controlString = new char[file_end_byte+1];
	}
	catch (std::exception & e) 
	{
		sprintf_s(textMsg, MAX_MESSAGE_LENGTH,
			"Allocation of char array in READ_file failed with exception:\n%s\n", e.what());
		logMsg.PushMessage(textMsg, LOG_SEVERITY_ERROR);
		return(CLOSE_handleAndReturn(hFile, ERROR_BAD_DYNAMIC_ALLOCATION));
	}

	sprintf_s(textMsg, MAX_MESSAGE_LENGTH, "Try to ReadFile. Read whole file [%lu bytes] from [%s], \n", bytes2get, a_filePath);
	logMsg.PushMessage(textMsg, LOG_SEVERITY_READING_FILE);
	DWORD bytes_got;
	if (	 FALSE == ReadFile( hFile, (LPVOID) (G_controlString), bytes2get, &bytes_got, NULL) ) 
	{ 
		// Failed to ReadFile
		sprintf_s(textMsg, MAX_MESSAGE_LENGTH, "ERROR:\tFunction ReadFile failed with 0x%04x - returned FALSE\n", GetLastError());
		logMsg.PushMessage(textMsg, LOG_SEVERITY_ERROR);
		return(CLOSE_handleAndReturn(hFile, ERROR_READFILE_FAIL));
	}
	else if( bytes_got == 0){
		// reading beyond EOF
		logMsg.PushMessage("Reading ended = EOF", LOG_SEVERITY_READING_FILE);
	}
	sprintf_s(textMsg, MAX_MESSAGE_LENGTH, "bytes_got = %lu\n", bytes_got);	
	logMsg.PushMessage(textMsg, LOG_SEVERITY_READING_FILE);

	G_controlString[bytes_got] = '\0';

	sprintf_s(textMsg, MAX_MESSAGE_LENGTH, "[FILE_START]\n%s\n[FILE_END]",G_controlString);
	logMsg.PushMessage(textMsg, LOG_SEVERITY_NORMAL);

	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// CloseHandle
	CLOSE_handleAndReturn(hFile, FLAWLESS_EXECUTION);	

	return(FLAWLESS_EXECUTION);
}



/****************************************************************************
@function   CLOSE_handleAndReturn
@brief      
@param[in]  HANDLE a_handle
			DWORD error_sum
			bool a_writeError = true
@param[out] 
@return     
************/
DWORD CLOSE_handleAndReturn(HANDLE a_handle, DWORD error_sum, bool a_logError)
{
	char textMsg[MAX_MESSAGE_LENGTH]; // char array for printing messages
	if(a_logError)
	{
		logMsg.PushMessage("Try to CloseHandle.", LOG_SEVERITY_NORMAL);
	}
	if( CloseHandle(a_handle) == 0 )
	{
		//RtPrintf("Function CloseHandle failed with 0x%04x\n", GetLastError());
		sprintf_s(textMsg, MAX_MESSAGE_LENGTH, "Function CloseHandle failed with 0x%04x\n", GetLastError());
		logMsg.PushMessage(textMsg, LOG_SEVERITY_NORMAL);
		return(error_sum + ERROR_CLOSEHANDLE_FAIL);
	}
	else 
	{
		if(a_logError)
		{
			logMsg.PushMessage("Successfully closed handle", LOG_SEVERITY_NORMAL);
		}
		return(error_sum);
	}
}

/****************************************************************************
@function   MOVE_pointer
@brief      function moves the pointer in handled file 
			to distance relative to current position / absolute to start 
			as set by [MoveMethod]
			if the function fails -> closes the handle and returns error_sum
____________________________________________________
@param[in]  (HANDLE)a_hFile | handle to a file which is already open
			LONG a_distance2move | number of bytes to move 
			DWORD a_moveMethod=FILE_CURRENT | from which point to move
			(bool)a_get_file_current_byte=true 
			| if true, copy current file byte into a_file_current_byte
			| else let it be
			(bool)a_logError=true | to log errors
____________________________________________________
@param[out] (DWORD*) a_file_current_byte | viz [a_get_file_current_byte]
@return     (DWORD) returns error_sum / FLAWLESS_EXECUTION
************/
DWORD MOVE_pointer(HANDLE a_hFile, LONG a_distance2move, DWORD* a_file_current_byte, DWORD a_moveMethod )
{
	char textMsg[MAX_MESSAGE_LENGTH]; // char array for printing messages
	logMsg.PushMessage("Try to SetFilePointer.\n", SEVERITY_MAX - 4);
	DWORD file_current_byte = SetFilePointer(a_hFile, a_distance2move, NULL, a_moveMethod);

	if(file_current_byte == INVALID_SET_FILE_POINTER) 
	{ // Failed to SetFilePointer
		sprintf_s(textMsg, MAX_MESSAGE_LENGTH, "Function SetFilePointer failed with 0x%04x\n", GetLastError());
		logMsg.PushMessage(textMsg, LOG_SEVERITY_NORMAL);
		return(CLOSE_handleAndReturn(a_hFile, ERROR_SETFILEPOINTER_FAIL));
	}

	*a_file_current_byte = file_current_byte;

	sprintf_s(textMsg,"file_current_byte = %lu\n", file_current_byte);
	logMsg.PushMessage(textMsg,LOG_SEVERITY_READING_FILE);
	return(FLAWLESS_EXECUTION);
}

/****************************************************************************
@function   char2num
@brief      convert char represented digit into integer representation
@param[in]  (char)ch - input character
@param[out] -
@return     (int)
			|if the char is not a digit -> return ERROR_IS_NOT_NUMBER
			|else return the integer represetation 
************/
int char2num(char ch){
	int digit = (int)ch - (int)('0');	
	if(digit<0 || digit>=10)
		return(ERROR_IS_NOT_NUMBER);
	else
		return(digit);
}