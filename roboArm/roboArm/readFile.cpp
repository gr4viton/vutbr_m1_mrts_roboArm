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

/****************************************************************************
@function   READ_patialConfigurationFromFile
@brief      	parsing out the parameters for individual phases from the string [str]
@param[in]  
@param[out] 
@return     (int)
			on Success	= FLAWLESS_EXECUTION
			on Error		= error_sum of ERRORS defined in returnCodeDefines.h
************/
DWORD READ_spatialConfigurationFromFile(C_roboticManipulator* a_manip, char* a_filePath){
	if(*a_filePath == 0 && a_manip->IS_in_bounds(1)) return(100000);
	/*
	// char array for printing messages
	char textMsg[MAX_MESSAGE_LENGTH];
	DWORD error_sum = 0;
	//____________________________________________________
	// read control file into string
	error_sum = READ_file(a_filePath);
	if(error_sum != FLAWLESS_EXECUTION)
	{
		delete[] G_controlString;
		//printf("READ_file failed with error_sum %lu\n", error_sum);
		sprintf_s(textMsg, MAX_MESSAGE_LENGTH, "READ_file failed with error_sum %lu\n", error_sum);
		logMsg.PushMessage(textMsg, PUSHMSG_SEVERITY_NORMAL);
		return(error_sum);
	}
	//____________________________________________________
	// parse control string into individual phases
	error_sum = PARSE_controlString(&(*a_manip));
	if(error_sum != FLAWLESS_EXECUTION)
	{
		delete[] G_controlString;
		//printf("READ_file failed with error_sum %lu\n", error_sum);
		sprintf_s(textMsg, MAX_MESSAGE_LENGTH, "READ_file failed with error_sum %lu\n", error_sum);
		logMsg.PushMessage(textMsg, PUSHMSG_SEVERITY_NORMAL);
		return(error_sum);
	}
	// AFTER creation of new prvek in array of C_spatialConf you must copy non-changed angles from previous phase
	delete[] G_controlString;
	//return(a_ROB->CONVERT_angle2intervalOne(i);*/
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
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>!!!<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
// rewrite without string
// implement MAX_WAIT_TIME_CMD_NUM_OF_DIGITS
// only 4 digits of angle should be readed
// only MAX_WAIT_TIME_CMD_NUM_OF_DIGITS shoudl be readed
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>!!!<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
DWORD PARSE_controlString(C_roboticManipulator* a_manip){
	if(a_manip->IS_in_bounds(1)) return(100000);
	/*
	// char array for printing messages
	char textMsg[MAX_MESSAGE_LENGTH];

	C_roboticManipulator* ROB = (C_roboticManipulator*)a_manip;
	
	std::string controlString = std::string(G_controlString);
	std::string delimiter = ";";
	size_t pos = 0; // position of a char in string
	std::string token = "";

	//first phase
//	ROB->phases.push_back(C_spatialConfiguration());
	int i_serv = 0;
	int int_value = 0;
	int int_from_char = 0;
	LARGE_INTEGER LI_value ; LI_value.QuadPart = 0;
	std::size_t found_position = 0; // index of found control chars
	char control_chars_string[] = "W=>";
	char found_control_char = '\0';
	

	while ((pos = controlString.find(delimiter)) != std::string::npos) 
	{
		DWORD error_sum = FLAWLESS_EXECUTION;
		token = controlString.substr(0, pos);
		//____________________________________________________
		// Wtime
		found_control_char = '\0';
		// search for individual control_chars in token string
		for(int i=0; control_chars_string[i]!='\0'; i++)
		{
			found_position = token.find_first_of(control_chars_string[i]);
			if(found_position != std::string::npos) 
			{ // found some control_char in token string
				found_control_char = control_chars_string[i];
				break;
			}
			else
			{ // did not find control_chars_string[i] in token string
				if(control_chars_string[i+1]=='\0')
				{ // did not found any control char -> bad or void line
					error_sum = ERROR_INCONSISTENT_FILE_LINE;
				}
			}
		}

		if(error_sum == ERROR_INCONSISTENT_FILE_LINE){
#ifndef IGNORE_INCONSISTENT_FILE_LINE //if NOT defined
			return(ERROR_INCONSISTENT_FILE_LINE);
#endif
			//ifdef -> continue with next token
			continue;
		}

		switch(found_control_char){
		//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		case('W'): // phase waiting time = end of this phase -> create another one
			
			//ROB->phases.back().phaseInterval = 
//			ROB->phases.push_back(C_spatialConfiguration());
			break;
		//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		case('='): // i_serv=angle
			i_serv = char2num( token[found_position-1] );			
			if(i_serv == ERROR_IS_NOT_NUMBER)
			{ // i_serv is not a number
				printf("Parsed servo index is not a number in string \"%s\"\n",token.c_str());
#ifndef IGNORE_NOT_NUMBER_ANGLE_IN_CONTROL_FILE //if NOT defined
				return(ERROR_IS_NOT_NUMBER);
#endif		
				//printf("Continuing with next servo, because IGNORE_NOT_NUMBER_ANGLE_IN_CONTROL_FILE is defined\n");
				logMsg.PushMessage("Continuing with next servo, because IGNORE_NOT_NUMBER_ANGLE_IN_CONTROL_FILE is defined", PUSHMSG_SEVERITY_NORMAL);
			}
			else
			{ // i_serv is a number
				error_sum = ROB->IS_in_bounds(i_serv);
				if( error_sum == FLAWLESS_EXECUTION)
				{ // i_serv is in bounds
					
					// token = (string) angle
					//token = std::string(token.substr(found_position+1));
					std::string str_angle = token.substr(found_position+1);
					//token.append('\0');
					bool done = false;
					int_value = 0;
					
					//____________________________________________________
					// parse angle string into int 
					std::size_t i = 0;
					std::size_t str_angle_size = str_angle.size();
					if(str_angle.length()!=0)
					{
						while(!done){
							int_from_char = char2num( str_angle[i] );
							if(int_from_char == ERROR_IS_NOT_NUMBER) 
							{
								done = true;
								//printf("One of digits from angle [%s] is not a number!\n",token.c_str());
								sprintf_s(textMsg, MAX_MESSAGE_LENGTH, "One of digits from angle [%s] is not a number!\n",token.c_str());
								logMsg.PushMessage(textMsg, PUSHMSG_SEVERITY_NORMAL);
								return(ERROR_IS_NOT_NUMBER);
							}
							int_value*=10;
							int_value+=int_from_char;
							i++;
							if(i >= str_angle_size) done = true;
						}
					//____________________________________________________
					// int_value = (int) angle
					ROB->CONVERT_angle2intervalOne(int_value, i_serv, &LI_value);
//					ROB->phases.back().SET_servIntervalOne(i_serv,	&LI_value);
					}
					else
					{ // token is empty
						return(error_sum);
					}
				}
				else
				{ //i_serv is out of bounds 
					return(error_sum);
				}
			}
		}
		// wrap
		
		//std::cout << token << std::endl;
		//printf("token = \"%s\"\n", token.c_str());
		sprintf_s(textMsg, MAX_MESSAGE_LENGTH, "token = \"%s\"\n", token.c_str());
		logMsg.PushMessage(textMsg, PUSHMSG_SEVERITY_NORMAL);
		controlString.erase(0, pos + delimiter.length());
	}
	// last
  //  std::cout << controlString << std::endl;
	//printf("%s\n",controlString.c_str());
	sprintf_s(textMsg, MAX_MESSAGE_LENGTH, "%s",controlString.c_str());
	logMsg.PushMessage(textMsg, PUSHMSG_SEVERITY_NORMAL);

	ROB->RESET_DOport();
	ROB = NULL;
	*/
	return(FLAWLESS_EXECUTION);
}
/****************************************************************************
@function   
@brief      
@param[in]  
@param[out] 
@return     error_sum
************/
DWORD CREATE_file(HANDLE* a_hFile)
{
	return(FLAWLESS_EXECUTION);
	/*
	hFile = CreateFile(a_filePath, GENERIC_READ, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE) { // Failed CreateFile
		//LogMessage()
		// ifdef
		//RtPrintf("Function CreateFile failed with 0x%04x - INVALID_HANDLE_VALUE\n", GetLastError());
		sprintf_s(textMsg, MAX_MESSAGE_LENGTH, "Function CreateFile failed with 0x%04x - INVALID_HANDLE_VALUE\n", GetLastError());
		logMsg.PushMessage(textMsg, PUSHMSG_SEVERITY_NORMAL);
		return(ERROR_CREATEFILE_FAIL);
	}*/
}


/****************************************************************************
@function   READ_file
@brief      routine for reading out control txt file into string [str]
@param[in]  (char*)a_filePath
@param[out] 
@return     	on Success	= FLAWLESS_EXECUTION
			on Error		= error_sum of ERRORS defined in returnCodeDefines.h
***************/
DWORD READ_file(char* a_filePath){
	// char array for printing messages
	char textMsg[MAX_MESSAGE_LENGTH];
	DWORD error_sum = 0;
	HANDLE hFile = NULL;
	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// CreateFile - for read handle 
#ifdef DEBUG_PRINT_READ_FUNCTIONS
	//RtPrintf("Try to CreateFile.\n");
	logMsg.PushMessage("Try to CreateFile.", SEVERITY_MAX - 4);
#endif
	// CONST CHAR * = LPCSTR 
	//char file_path[] = "D:\\EDUC\\m1\\R_MRTS\\float.txt";

	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>!!!<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	//rewrite to function
	//hFile = CREATE_file(hFile..
	hFile = CreateFile(a_filePath, GENERIC_READ, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE) { // Failed CreateFile
		//LogMessage()
		// ifdef
		//RtPrintf("Function CreateFile failed with 0x%04x - INVALID_HANDLE_VALUE\n", GetLastError());
		sprintf_s(textMsg, MAX_MESSAGE_LENGTH, "Function CreateFile failed with 0x%04x - INVALID_HANDLE_VALUE\n", GetLastError());
		logMsg.PushMessage(textMsg, PUSHMSG_SEVERITY_NORMAL);
		return(ERROR_CREATEFILE_FAIL);
	}
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>!!!<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

#ifdef DEBUG_PRINT_READ_FUNCTIONS
	//RtPrintf("CreateFile completed successfully.\n");
	logMsg.PushMessage("CreateFile completed successfully.", SEVERITY_MAX - 4);
#endif
	
	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// SetFilePointer
	DWORD file_end_byte = 0;
#ifdef DEBUG_PRINT_READ_FUNCTIONS
	RtPrintf("Try to SetFilePointer to FILE_END:\n");
	logMsg.PushMessage("Try to SetFilePointer to FILE_END:", SEVERITY_MAX - 4);
#endif
	error_sum = MOVE_pointer(hFile, 0, &file_end_byte, FILE_END);
	if(error_sum != FLAWLESS_EXECUTION)	return(CLOSE_handleAndReturn(hFile, error_sum));
	
	DWORD file_begin_byte = 0;
#ifdef DEBUG_PRINT_READ_FUNCTIONS
	RtPrintf("Try to SetFilePointer to FILE_BEGIN:\n");
	logMsg.PushMessage("Try to SetFilePointer to FILE_BEGIN:", SEVERITY_MAX - 4);
#endif
	error_sum = MOVE_pointer(hFile, 0, &file_begin_byte, FILE_BEGIN);
	if(error_sum != FLAWLESS_EXECUTION)	return(CLOSE_handleAndReturn(hFile, error_sum));

	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// ReadFile

	//DWORD file_current_byte = file_begin_byte;
	//unsigned long max = 0;
	//max - 1;

	//DWORD bytes2get = FILE_MAX_CHARS;
	DWORD bytes2get = file_end_byte;
	try	{
		G_controlString = new char[file_end_byte+1];
	}
	catch (std::exception & e) {
		printf("Allocation of char array in READ_file failed with exception:\n%s\n", e.what());
		return(CLOSE_handleAndReturn(hFile, ERROR_BAD_DYNAMIC_ALLOCATION));
	}

#ifdef DEBUG_PRINT_READ_FUNCTIONS
//	RtPrintf("Try to READ_chunk [%lu bytes] from file.\n",bytes2get);
	//RtPrintf("Try to ReadFile. Read whole file [%lu bytes] from [%s], \n", bytes2get, a_filePath);
	sprintf_s(textMsg, MAX_MESSAGE_LENGTH, "Try to ReadFile. Read whole file [%lu bytes] from [%s], \n", bytes2get, a_filePath);
	logMsg.PushMessage(textMsg, PUSHMSG_SEVERITY_NORMAL);
#endif
	DWORD bytes_got;
	// BOOL ReadFile(HANDLE hFile, LPVOID lpBuffer, DWORD nbytes2get, LPDWORD lpbytes_got, LPOVERLAPPED lpOverlapped);
	if (	 FALSE == ReadFile( hFile, (LPVOID) (G_controlString), bytes2get, &bytes_got, NULL) ) 
	{ // Failed to ReadFile
		//RtPrintf("ERROR:\tFunction ReadFile failed with 0x%04x - returned FALSE\n", GetLastError());
		sprintf_s(textMsg, MAX_MESSAGE_LENGTH, "ERROR:\tFunction ReadFile failed with 0x%04x - returned FALSE\n", GetLastError());
		logMsg.PushMessage(textMsg, PUSHMSG_SEVERITY_NORMAL);
		return(CLOSE_handleAndReturn(hFile, ERROR_READFILE_FAIL));
	}
	else if( bytes_got == 0){
		// reading beyond EOF
#ifdef DEBUG_PRINT_READ_FUNCTIONS
		RtPrintf("Reading ended = EOF\n");
		logMsg.PushMessage("Reading ended = EOF", SEVERITY_MAX - 4);
#endif
	}
#ifdef DEBUG_PRINT_READ_FUNCTIONS
	//RtPrintf("bytes_got = %lu\n", bytes_got);	
	sprintf_s(textMsg, MAX_MESSAGE_LENGTH, "bytes_got = %lu\n", bytes_got);	
	logMsg.PushMessage(textMsg, SEVERITY_MAX - 4);
#endif
	G_controlString[bytes_got] = '\0';
	//printf("[FILE_START]\n%s\n[FILE_END]",G_controlString);
	sprintf_s(textMsg, MAX_MESSAGE_LENGTH, "[FILE_START]\n%s\n[FILE_END]",G_controlString);
	logMsg.PushMessage(textMsg, PUSHMSG_SEVERITY_NORMAL);

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
		logMsg.PushMessage("Try to CloseHandle.", PUSHMSG_SEVERITY_NORMAL);
	}
	if( CloseHandle(a_handle) == 0 )
	{
		//RtPrintf("Function CloseHandle failed with 0x%04x\n", GetLastError());
		sprintf_s(textMsg, MAX_MESSAGE_LENGTH, "Function CloseHandle failed with 0x%04x\n", GetLastError());
		logMsg.PushMessage(textMsg, PUSHMSG_SEVERITY_NORMAL);
		return(error_sum + ERROR_CLOSEHANDLE_FAIL);
	}
	else 
	{
		if(a_logError)
		{
			logMsg.PushMessage("Successfully closed handle", PUSHMSG_SEVERITY_NORMAL);
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
DWORD MOVE_pointer(HANDLE a_hFile, LONG a_distance2move, DWORD* a_file_current_byte, 
	DWORD a_moveMethod, bool a_get_file_current_byte, bool a_logError)
{
	char textMsg[MAX_MESSAGE_LENGTH]; // char array for printing messages
	if(a_logError)
		logMsg.PushMessage("Try to SetFilePointer.\n", SEVERITY_MAX - 4);
	DWORD file_current_byte = SetFilePointer(a_hFile, a_distance2move, NULL, a_moveMethod);
	if(a_get_file_current_byte)
	{
		*a_file_current_byte = file_current_byte;
	}
	if(file_current_byte == INVALID_SET_FILE_POINTER) 
	{ // Failed to SetFilePointer
		if(a_logError)
		{
			sprintf_s(textMsg, MAX_MESSAGE_LENGTH, "Function SetFilePointer failed with 0x%04x\n", GetLastError());
			logMsg.PushMessage(textMsg, PUSHMSG_SEVERITY_NORMAL);
		}
		return(CLOSE_handleAndReturn(a_hFile, ERROR_SETFILEPOINTER_FAIL));
	}
#ifdef DEBUG_PRINT_READ_FUNCTIONS
	logMsg.PushMessage("file_current_byte = %lu\n",*file_current_byte);
#endif
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
	if(digit<0 || digit>9)
		return(ERROR_IS_NOT_NUMBER);
	else
		return(digit);
}