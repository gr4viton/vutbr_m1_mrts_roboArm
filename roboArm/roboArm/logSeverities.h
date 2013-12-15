/***************
@project  roboArm
@filename logSeverities.h
@author   xdavid10, xslizj00 @ FEEC-VUTBR 
@date     2013_12_02
@brief    file containing SEVERITY definitions for logging message function
***************/

#ifndef LOGSEVERITIES_H
#define LOGSEVERITIES_H

//____________________________________________________
// severities
// SEVERITY VALUES
#define 	LOG_SEVERITY_VALUE_MIN				SEVERITY_MIN
#define 	LOG_SEVERITY_VALUE_LOWEST			SEVERITY_MAX - 9
#define 	LOG_SEVERITY_VALUE_LOWER				SEVERITY_MAX - 8
#define 	LOG_SEVERITY_VALUE_LOW				SEVERITY_MAX - 7 
#define LOG_SEVERITY_VALUE_MEDIUM			SEVERITY_MAX - 5	
#define 	LOG_SEVERITY_VALUE_HIGH				SEVERITY_MAX - 3
#define 	LOG_SEVERITY_VALUE_HIGHER			SEVERITY_MAX - 2
#define 	LOG_SEVERITY_VALUE_HIGHEST			SEVERITY_MAX - 1

// one parameter should be added to PushMsg -> function where it is called from 
// -> create typedef enum with strings of functions !!

//____________________________________________________
// LOG_SEVERITIES - used in program
// Severe errors logs with this value - (overwriting local severity)
#define LOG_SEVERITY_ERROR					LOG_SEVERITY_VALUE_HIGHEST

#define LOG_SEVERITY_EXITING_PROCESS			LOG_SEVERITY_VALUE_HIGHER
#define LOG_SEVERITY_EXITING_THREAD			LOG_SEVERITY_VALUE_HIGH
#define LOG_SEVERITY_LOGGING_STARTED			LOG_SEVERITY_VALUE_HIGHEST

// thread PWM severities
#define LOG_SEVERITY_PWM_TIC					LOG_SEVERITY_VALUE_LOWEST
#define LOG_SEVERITY_PWM_PERIOD				LOG_SEVERITY_VALUE_LOWER
#define LOG_SEVERITY_PWM_PHASE				LOG_SEVERITY_VALUE_MEDIUM
#define LOG_SEVERITY_READING_FILE			LOG_SEVERITY_VALUE_MEDIUM

// other functions severities
#define LOG_SEVERITY_INIT					LOG_SEVERITY_VALUE_HIGHER
#define LOG_SEVERITY_MAIN_FUNCTION			LOG_SEVERITY_VALUE_HIGHER

// other - normal log messages
#define LOG_SEVERITY_NORMAL					LOG_SEVERITY_VALUE_MEDIUM		

#endif