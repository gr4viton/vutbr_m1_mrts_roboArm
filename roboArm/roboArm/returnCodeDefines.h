/***************
@project  roboArm
@filename returnCodeDefines.h
@author   xdavid10, xslizj00 @ FEEC-VUTBR 
@date     2013_12_02
@brief    file containing definition of return codes from threads and functions
***************/

#ifndef C_RETURNCODEDEFINES_H
#define C_RETURNCODEDEFINES_H

// divide into three sections

// function error code
// FERROR_

// thread error code
// TERROR_

// process error code
// PERROR_

// ____________________________________________________
// error_sum
#define FLAWLESS_EXECUTION						0
#define DESTRUCT_EVERYTHING						1
#define ERR_SERVO_INDEX_OUT_OF_BOUNDS			2
#define ERR_C_SERVOMOTOR_SETTIMERREL_ERROR		3
#define ERR_C_SERVOMOTOR_TIMER_INVALID_HANDLE	4
#define ERR_INIT_CANNOT_LOAD_LIBRARY				5
#define ERR_CONSTRUCOR_ERROR_OFFSET				127
// free positions offset + max_servo_i

//____________________________________________________
// ExitProcess return value constants - ERRORS & etc
#define SUCCESSFUL_END								0
#define ERROR_CMESSAGE_TOO_LONG						1
#define ERROR_CREATEFILE_FAIL						2
#define ERROR_SETFILEPOINTER_FAIL					4
#define ERROR_READFILE_FAIL							8
#define ERROR_WRITEFILE_FAIL							16
#define ERROR_CLOSEHANDLE_FAIL						32
#define ERROR_FILE_PATH_STRING_TOO_LONG				64
//
#define ERROR_COULD_NOT_TERMINATE_THREAD				128
#define ERROR_COULD_NOT_RESUME_THREAD				256
#define ERROR_COULD_NOT_CHANGE_PRIORITY				512
#define ERROR_COULD_NOT_CREATE_THREAD				1024

//____________________________________________________
//LogMessageA
#define ERROR_SPRINTF_S								98002
#define ERROR_SEVERITY_LOWER_THAN_MIN				98003
#define ERROR_SEVERITY_BIGGER_THAN_MAX				98004
#define LOG_IS_NOT_SEVERE_ENAUGH						98005
#define ERROR_COULD_NOT_GET_CLOCKTIME				98006
#define ERROR_FILETIMETOSYSTEMTIME_FAIL				98007
#define ERROR_LOGGING_IS_LOCKED						98008
#define ERROR_BUFFER_IS_EMPTY						98009
#define ERROR_NOT_ENAUGH_SPACE_IN_BUFFER				98010

//____________________________________________________
// C_spatialConfiguration
#define ERROR_ANGLE_OUT_OF_BOUNDS					99256
#define ERROR_CONTROLFILE_PATH_NOT_SPECIFIED			99257
#define ERROR_BAD_DYNAMIC_ALLOCATION					99258
#define ERROR_IS_NOT_NUMBER							99259
#define ERROR_INCONSISTENT_FILE_LINE					99260
//____________________________________________________
#define EXITCODE_SUCCESSFUL_END					0
#define EXITCODE_TERMINATED_BY_MAIN				1




#endif