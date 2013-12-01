/**
 * @description Logging module for MRTS aplications
 * @author Bc. Jiøí Sliž <xslizj00@stud.feec.vutbr.cz>
 *
 *
 */


#include <windows.h>
#include <wchar.h>
#include <rtapi.h>
#include <stdio.h>
#include <string.h>
//#include <ctype.h>
//#include <conio.h>
//#include <stdlib.h>
//#include <math.h>
//#include <errno.h>
#include <WinError.h>

#define SEVERITY_MIN 0
#define SEVERITY_MAX 16
#define SEVERITY_LEVEL 7

#define LOG_FILE "C:\\mrts\\xslizj00\\cv6\\LogMessage.txt"
#define LOG_SCREEN

/**
 * @name 	LogMessage
 * @brief	function logs a message into the application log stream in the format: DD.MM.YYYY HH:MM:SS:MSS Message
 * @param [in]	iSeverity - an importance of the message in the range <SEVERITY_MIN, SEVERITY_MAX>. Messages lower than SEVERITY_LEVEL are not being logged
 * @param [in]	cMessage - a message to be logged
 * @param [in]	bBlocking - if 0 the function is blocking (synchronous write operation), otherwise function is non-blocking (asynchronous write operation with buffering)
 * @retval	0 - function succeeded
 * @retval	otherwise - an error occurred
*/
int LogMessage(int iSeverity, char *cMessage, int bBlocking);