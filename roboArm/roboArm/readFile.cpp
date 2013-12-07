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
		delete[] G_controlString;
		printf("READ_file failed with error_sum %i\n", error_sum);
		return(error_sum);
	}
	//____________________________________________________
	// parse control string into individual phases
	error_sum = PARSE_controlString(&(*a_manip));
	if(error_sum != FLAWLESS_EXECUTION)
	{
		delete[] G_controlString;
		printf("READ_file failed with error_sum %i\n", error_sum);
		return(error_sum);
	}
	// AFTER creation of new prvek in array of C_spatialConf you must copy non-changed angles from previous phase
	delete[] G_controlString;
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
	
	std::string controlString = std::string(G_controlString);
	std::string delimiter = ";";
	size_t pos = 0; // position of a char in string
	std::string token = "";

	//first phase
	ROB->phases.push_back(C_spatialConfiguration());
	int i_serv = 0;
	int int_value = 0;
	int int_from_char = 0;
	LARGE_INTEGER LI_value ; LI_value.QuadPart = 0;
	std::size_t found_position = 0; // index of found control chars
	char *control_chars_string = "W=>";
	char found_control_char = '\0';
	

	while ((pos = controlString.find(delimiter)) != std::string::npos) 
	{
		int error_sum = FLAWLESS_EXECUTION;
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
			ROB->phases.push_back(C_spatialConfiguration());
			break;
		//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		case('='): // i_serv=angle
			i_serv = char2num( token[found_position-1] );			
			if(i_serv == ERROR_IS_NOT_NUMBER)
			{ // i_serv is not a number
				printf("Parsed servo index is not a number in string \"%s\"\n",token);
#ifndef IGNORE_NOT_NUMBER_ANGLE_IN_CONTROL_FILE //if NOT defined
				return(ERROR_IS_NOT_NUMBER);
#endif		
				printf("Continuing with next servo, because IGNORE_NOT_NUMBER_ANGLE_IN_CONTROL_FILE is defined\n");
			}
			else
			{ // i_serv is a number
				error_sum = ROB->IS_in_bounds(i_serv);
				if( error_sum == FLAWLESS_EXECUTION)
				{
					// token = (string) angle
					token = token.substr(found_position+1);
					bool done = false;
					int_value = 0;
					//____________________________________________________
					// parse angle string into int
					int i = 0;
					while(!done){
						int_from_char = char2num( token[i] );
						if(int_from_char == ERROR_IS_NOT_NUMBER) 
						{
							done = true;
							printf("One of digits from angle [%s] is not a number!\n",token);
							return(ERROR_IS_NOT_NUMBER);
						}
						int_value*=10;
						int_value+=int_from_char;
						i++;
					}
					//____________________________________________________
					// int_value = (int) angle
					ROB->CONVERT_angle2int_zero(int_value, i_serv, &LI_value);
					ROB->phases.back().SET_servIntervalZero(i_serv,	&LI_value);
				}
				else
				{ //i_serv is out of bounds 
					return(error_sum);
				}
			}
		}
		// wrap
		
		//std::cout << token << std::endl;
		printf("token = \"%s\"\n", token);
		controlString.erase(0, pos + delimiter.length());
	}
	// last
  //  std::cout << controlString << std::endl;
	printf("%s\n",controlString);

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
		G_controlString = new char[file_end_byte+1];
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
	if (	 FALSE == ReadFile( hFile, (LPVOID) (G_controlString), bytes2get, &bytes_got, NULL) ) 
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
	G_controlString[bytes_got] = '\0';
	printf("[FILE_START]\n%s\n[FILE_END]",G_controlString);

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

/****************************************************************************
@function   char2num
@brief      
@param[in]  
@param[out] 
@return     
************/
int char2num(char ch){
	int digit = (int)ch - (int)('0');
	if(digit<0 || digit>9)
		return(ERROR_IS_NOT_NUMBER);
	else
		return(digit);
}