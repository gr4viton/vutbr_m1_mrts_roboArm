/***************
@project  roboArm
@filename unused.h
@author   xdavid10, xslizj00 @ FEEC-VUTBR 
@date     2013_12_07
@brief    unused functions
***************/



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
				logMsg.PushMessage("Continuing with next servo, because IGNORE_NOT_NUMBER_ANGLE_IN_CONTROL_FILE is defined", LOG_SEVERITY_NORMAL);
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
								logMsg.PushMessage(textMsg, LOG_SEVERITY_NORMAL);
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
//					ROB->phases.back().SET_serv_intervalOne(i_serv,	&LI_value);
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
		sprintf_s(textMsg, MAX_MESSAGE_LENGTH, "token = \"%s\"\n", token.c_str());
		logMsg.PushMessage(textMsg, LOG_SEVERITY_NORMAL);
		controlString.erase(0, pos + delimiter.length());
	}
	// last
	//printf("%s\n",controlString.c_str());
	sprintf_s(textMsg, MAX_MESSAGE_LENGTH, "%s",controlString.c_str());
	logMsg.PushMessage(textMsg, LOG_SEVERITY_NORMAL);

	ROB->RESET_DOport();
	ROB = NULL;
	return(FLAWLESS_EXECUTION);
}
#endif // RUNNING_ON_RTX64 - if NOT defined
