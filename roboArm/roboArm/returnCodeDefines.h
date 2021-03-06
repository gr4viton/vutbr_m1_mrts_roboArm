/***************
@project  roboArm
@filename returnCodeDefines.h
@author   xdavid10, xslizj00 @ FEEC-VUTBR 
@date     2013_12_02
@brief    file containing definition of return codes from threads and functions
***************/

#ifndef RETURNCODEDEFINES_H
#define RETURNCODEDEFINES_H

// divide into three sections

// function error code
// FERROR_

// thread error code
// TERROR_

// process error code
// PERROR_

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// error_sum

//____________________________________________________
// ExitProcess return value constants - ERRORS & etc
#define FLAWLESS_EXECUTION							0


#define ERROR_CMESSAGE_TOO_LONG						(1<<4)
#define ERROR_CREATEFILE_FAIL						(1<<5)
#define ERROR_SETFILEPOINTER_FAIL					(1<<6)
#define ERROR_READFILE_FAIL							(1<<7)
#define ERROR_WRITEFILE_FAIL							(1<<8)
#define ERROR_CLOSEHANDLE_FAIL						(1<<9)
#define ERROR_FILE_PATH_STRING_TOO_LONG				(1<<10)

#define ERROR_COULD_NOT_RESUME_THREAD				(1<<11)
#define ERROR_COULD_NOT_CHANGE_PRIORITY				(1<<12)
#define ERROR_COULD_NOT_CREATE_THREAD				(1<<13)

// TERMINATE all threads
#define ERROR_COULD_NOT_TERMINATE_THREAD				(1)
#define ERROR_COULD_NOT_TERMINATE_THREAD_OFFSET		(1<<14)

// ____________________________________________________
// C_roboticManipulator
#define DESTRUCT_EVERYTHING							96001
#define ERR_SERVO_INDEX_OUT_OF_BOUNDS				96002
#define ERR_C_SERVOMOTOR_SETTIMERREL_ERROR			96003
#define ERR_C_SERVOMOTOR_TIMER_INVALID_HANDLE		96004
#define ERR_INIT_CANNOT_LOAD_LIBRARY					96005
#define ERROR_SERVO_SET_CONSTANTS_FAIL				96006
#define ERROR_THIS_PHASE_IS_VOID						96007

#define NEXT_PHASE_IS_VOID							96008
#define EXITCODE_TERMINATED_BY_MAIN					96999
//____________________________________________________
// INIT
#define ERROR_COULD_NOT_LOAD_DAC_DLL_RTDLL_LIBRARY  97000
#define ERROR_COULD_NOT_FIND_PROC_ADDRESS			97001

//____________________________________________________
//LogMessageA
#define ERROR_SPRINTF_S_FAIL							98002
#define ERROR_SEVERITY_LOWER_THAN_MIN				98003
#define ERROR_SEVERITY_BIGGER_THAN_MAX				98004
#define LOG_IS_NOT_SEVERE_ENAUGH						98005
#define ERROR_COULD_NOT_GET_CLOCKTIME				98006
#define ERROR_FILETIMETOSYSTEMTIME_FAIL				98007
#define ERROR_LOGGING_IS_LOCKED						98008
#define ERROR_BUFFER_IS_EMPTY						98009
#define ERROR_NOT_ENAUGH_SPACE_IN_BUFFER				98010
#define ERROR_STRING_TO_WRITE_IS_TOO_LONG			98011
#define ERROR_STRING_LENGHT_LARGER_THAN_TRESHOLD		98012
//____________________________________________________
// C_spatialConfiguration
#define ERROR_ANGLE_OUT_OF_BOUNDS					99256
#define ERROR_CONTROLFILE_PATH_NOT_SPECIFIED			99257
#define ERROR_BAD_DYNAMIC_ALLOCATION					99258
#define ERROR_IS_NOT_NUMBER							99259
#define ERROR_INCONSISTENT_FILE_LINE					99260


#endif