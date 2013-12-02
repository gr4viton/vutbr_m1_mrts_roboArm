/***************
* @filename		roboArm.cpp
* @author 		xdavid10, xslizj00 @ FEEC-VUTBR 
* @contacts		Bc. Jiøí Sliž		<xslizj00@stud.feec.vutbr.cz>
				Bc. Daniel Davídek	<danieldavidek@gmail.com>
* @date			2013_12_02
* @brief		main file
* @description	Program for communication and control of PIO821 card, connected to a robotic manipulator ROB2-6.
				It reads the instructions from text file and sets the servo-mechanisms' angles respectively with
				predefined interval of halt time.
				For three of total six servos, there is time-ramp control implemented as there are feedback loop
				conected to the module.
***************/

#include "roboArm.h"

//____________________________________________________ 
// global Variables

DWORD baseAddress = 0;
HMODULE hLibModule = NULL;

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// mean of c values
DWORD MEAN_adc(int channel, int gain, int c){
	if(c==0) return(0);
	DWORD sum=0;
	//DWORD *vals;
	//vals = (DWORD*)malloc(sizeof(DWORD));
	int i = c;
	for(i;i>=0;i--){
		//vals[c-1] = GET_ADC(channel);
		sum += GET_ADC(channel, gain);
	}
	//free(vals);
	return(sum/c);
}

/****************************************************************************
* @function		PWM_dutyCycle
* @brief			periodically executed function for PWM on predefined DO port .
				Servo motors have their position regulated by pulses of different width.
* @param[in]		void *a_struct
				- i will not be needed
***************/
void RTFCNDCL PWM_dutyCycle(void *a_servoMotor)
{
	/*
	// writing to a critical section should be treated wisely ! <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	// is the register critical section? I think yess
	// mutex for individual bytes / bites of shadow-register
	// and one core thread reading shadow-register and writing it to the true byteAdress 
	// --- possibly event driven ---

	// so RtWritePortUchar -> will become something like WriteIntentionBit which will handle the mutexes right
	RtWritePortUchar(servoMotorByteAddress, 1<<servoMotorDigit);
	RtSleepFt(&interval_one);
	// You cannot write zeros everywhere
	RtWritePortUchar((PUCHAR)(baseAddress+DO_Low_Byte), 0x00);
	RtSleepFt(&interval_zero);*/
	LARGE_INTEGER interval_zero; interval_zero.QuadPart = 10*NS100_1MS;
	int i = 100;
	while(--i){
		RtPrintf("%i\n",i);
		RtSleepFt(&interval_zero);
	}
}


//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// CLOSE_handle
void CLOSE_handle(HANDLE handle, int error_sum){
	if( CloseHandle(handle) == 0 ){
		RtPrintf("Function CloseHandle failed with 0x%04x\n", GetLastError());
		ExitThread(error_sum + SUMFLOATS_ERROR_CLOSEHANDLE_FAIL);
	}
	else if(error_sum != 0)
		ExitThread(error_sum);
	else 
		return;
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// TERMINATE_allThreadsAndExitProcess
void TERMINATE_allThreadsAndExitProcess(HANDLE *hTh, int iTh_max, int error_sum){
	RtPrintf("Terminating all the threads\n");
	for(int iTh = 0; iTh<iTh_max; iTh++){
		if(FALSE == TerminateThread(hTh[iTh], EXITCODE_TERMINATED_BY_MAIN)){
			error_sum += SUMFLOATS_ERROR_COULD_NOT_TERMINATE_THREAD;
		}
		RtPrintf("> Thread %i successfuly closed\n", iTh);
		CLOSE_handle(hTh[iTh],error_sum);
	}
}


HANDLE hTh[SUM_SERVOMOTORS];	

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// main
void _cdecl main(int  argc, char **argv, char **envp)
{
	//double angles[6];
	int ret_val = 0;
	int i = 0;
	UCHAR pom = 0;
	//DWORD data;
	int num = 5; // number_of_mean_values

	printf("--------(: Clean start :)------\n");
	// ____________________________________________________
	ret_val = INIT_All();
	if(ret_val!=FLAWLESS_EXECUTION){
		//log
		ExitProcess(ret_val);
	}
	
	//____________________________________________________
	// init classes for the manipulator
	C_roboticManipulator ROB(ret_val);
	if(ret_val != FLAWLESS_EXECUTION)
	{
		//log
		ExitProcess(ret_val);
	}


	//____________________________________________________
	// thread creation
	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	const int iTh_max = SUM_SERVOMOTORS;
	DWORD this_loop_ExitCode_sum = 0;
	LPDWORD thExitCode[iTh_max];
//	void* thread_argument[iTh_max];

	// stack - set it properly small -> variable [str] in global
	int default_priority = RT_PRIORITY_MAX - 1;
	int wanted_priority = default_priority;
	int thread_priority = 0;
	int iTh = 0;
	
	DWORD thread_id = 0;

	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// RtCreateThread
	for(iTh = 0; iTh<iTh_max; iTh++){
		//____________________________________________________
		// handle
		RtPrintf("> Try to create multi thread %i.\n", iTh);
		hTh[iTh] = RtCreateThread(NULL, 0, 
			(LPTHREAD_START_ROUTINE) PWM_dutyCycle, (VOID*)iTh, CREATE_SUSPENDED, &thread_id);
		if(hTh[iTh] == NULL){
			RtPrintf("ERROR:\tCannot create thread %i.\n",iTh);
			TERMINATE_allThreadsAndExitProcess(hTh, iTh_max, SUMFLOATS_ERROR_COULD_NOT_CREATE_THREAD);
		}
		RtPrintf("Thread %i created and suspended with priority %i.\n", iTh, RtGetThreadPriority(hTh[iTh]) );

		// ____________________________________________________
		// RtSetThreadPriority
		wanted_priority = default_priority - iTh;
		if( RtSetThreadPriority( hTh[iTh], wanted_priority) ){
			thread_priority = RtGetThreadPriority(hTh[iTh]);
			if( thread_priority == wanted_priority ){
				RtPrintf("Priority of thread %i sucessfully set to %i\n", iTh, wanted_priority );
			}
			else{
				RtPrintf("ERROR:\tCannot set thread %i priority to %i! It currently has priority %i.\n", 
					iTh, wanted_priority , thread_priority);
				TERMINATE_allThreadsAndExitProcess(hTh, iTh_max, SUMFLOATS_ERROR_COULD_NOT_CHANGE_PRIORITY);
			}
		}
		else{
			RtPrintf("ERROR:\tCannot set thread %i priority to %i! It currently has priority %i.\n", 
				iTh, wanted_priority , GetThreadPriority(hTh[iTh]) );
			TERMINATE_allThreadsAndExitProcess(hTh, iTh_max, SUMFLOATS_ERROR_COULD_NOT_CHANGE_PRIORITY);
		}

	
		//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		// RtResumeThread
		if( RtResumeThread(hTh[iTh]) != 0xFFFFFFFF ){
			RtPrintf("Succesfully resumed thread %i.\n", iTh);
		}
		else{
			RtPrintf("Could not resume thread %i.\n", iTh);
			TERMINATE_allThreadsAndExitProcess(hTh, iTh_max, SUMFLOATS_ERROR_COULD_NOT_RESUME_THREAD);
		}
	}

	/*
	LARGE_INTEGER sleep; 
	sleep.QuadPart = 100000000;
		RtSleepFt(&sleep);
	*/
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// main thread-controlling super-loop
	int still_active_threads;
	LARGE_INTEGER preemptive_interval; 
	preemptive_interval.QuadPart = 100;
	iTh = 0;
	do{
		still_active_threads = 0;
		//BOOL GetExitCodeThread(HANDLE hThread, LPDWORD lpExitCode);
		if(GetExitCodeThread(hTh[iTh], (thExitCode[iTh]) ) == FALSE){
			RtPrintf("Function of thread %i failed, returned FALSE with exit-code %lu\n", iTh, *thExitCode);
			break;
		}
		if( *thExitCode[iTh] == STILL_ACTIVE ){
			still_active_threads++;
		}
		else{
			
		}
		//____________________________________________________
		// could not be executed if CPU>1 
		RtSleepFt(&preemptive_interval);
	}while(still_active_threads);
	
	for(iTh = 0; iTh<iTh_max; iTh++){
		RtPrintf("Thread %i terminated with exit code %lu\n", iTh, *thExitCode[iTh]);
		//printf("Thread %i sum = %f\n", iTh, static_cast<double *>(thread_argument[iTh]));
	}
	TERMINATE_allThreadsAndExitProcess(hTh, iTh_max, SUCCESSFUL_END);
	


	// TODO
	//____________________________________________________
	// 0) find out initial configurations for each servo in C_roboticManipulator constructor!
	// 1) thread creation for each servo (?in C_roboticManipulator constructor?)
	// --> REWRITE it as I thought it will be handled with timers, but
	// thread handling will be better
	
	// writing to a critical section should be treated wisely ! <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	// is the register critical section? I think yess
	// a) mutex for individual bytes / bites of shadow-register
	//		and one core thread reading shadow-register and writing it to the true byteAdress 
	// b) in each instance of class C_servoMotor there will be mutex for writing the intervals
	//		this mutex will be set/unset with the individual servoMotors threads
	//		and it will be read by the main thread every min_period of setting the register
	//		-> if it is closed the thread waits to write into it until main thread opens it agein
	// c) in each thread will add event when it wants to set/unset the byte
	//		main thread will treat this events and write to the critical-section = register
	// --- possibly event driven ---
	//

	// 2) member function PWM_dutyCycle -> periodically executed in each thread
	// 3) find out if writing to register is criticall section

	
	/*
	// is this secure? 
	// or I should be working only in C_roboticManipulator class to avoid encapsulation problems??
	C_servoMotor* pServo = NULL;
	ROB.GET_servoMotor(1, pServo);
	pServo->PWM_dutyCycle();
	*/


			
	LARGE_INTEGER interval_one; 
	LARGE_INTEGER interval_zero; 
	LARGE_INTEGER interval_wait; 
	
	interval_one.QuadPart = 10000; // 1000us
	interval_zero.QuadPart = 200000 - 10000; // 0.02s = 50Hz
	interval_wait.QuadPart = 10000000; // 1s

	int j = 0;
	int max_j = 8;
	//for(j = 0;j < max_j;j++){
	//	for(i = 2;i>0;i--)
	//	{
	//		//RtWritePortUchar((PUCHAR)(baseAddress+DO_High_Byte), 0x80);
	//		//RtWritePortUchar((PUCHAR)(baseAddress+DO_High_Byte), 255); // big
	//		//RtWritePortUchar((PUCHAR)(baseAddress+DO_High_Byte), 127);
	//		//RtWritePortUchar((PUCHAR)(baseAddress+DO_High_Byte), 63);
	//		//RtWritePortUchar((PUCHAR)(baseAddress+DO_High_Byte), 31);
	//		//RtWritePortUchar((PUCHAR)(baseAddress+DO_High_Byte), 15); // servo 
	//		//RtWritePortUchar((PUCHAR)(baseAddress+DO_High_Byte), 7); // servo 
	//		//HightByte 1<<7 = servo  1
	//		RtWritePortUchar((PUCHAR)(baseAddress+DO_High_Byte), 1<<j);
	//		RtSleepFt(&interval_one);
	//		RtWritePortUchar((PUCHAR)(baseAddress+DO_High_Byte), 0x00);
	//		RtSleepFt(&interval_zero);
	//	}
	//	printf("Servo %i/%i\n", j,max_j);
	//	RtSleepFt(&interval_wait);
	//}
	E_servos testedServo = S5;
	int period = 200;	//Hz
	interval_one.QuadPart = NS100_1US * 500;	// 500 us
	interval_zero.QuadPart = NS100_1MS * 1000/period - interval_one.QuadPart; // 0.01s = 100Hz
	interval_wait.QuadPart = NS100_1MS * 300; // 0.1s
	for( i = 0 ; i < 200 ; i++)
	{
		RtWritePortUchar((PUCHAR)(baseAddress+DO_High_Byte), 1<<testedServo);
		RtSleepFt(&interval_one);
		RtWritePortUchar((PUCHAR)(baseAddress+DO_High_Byte), 0x00);
		RtSleepFt(&interval_zero);
	}

	interval_one.QuadPart = NS100_1US * 2500;	// 2500 us
	interval_zero.QuadPart = NS100_1MS * 1000/period - interval_one.QuadPart; // 0.01s = 100Hz
	interval_wait.QuadPart = NS100_1MS * 300; // 0.1s
	for( i = 0 ; i < 200 ; i++)
	{
		RtWritePortUchar((PUCHAR)(baseAddress+DO_High_Byte), 1<<testedServo);
		RtSleepFt(&interval_one);
		RtWritePortUchar((PUCHAR)(baseAddress+DO_High_Byte), 0x00);
		RtSleepFt(&interval_zero);
	}
	// Reading Data 
	while(1)
	{
		/*
			MEAN_adc(0,0,num);
			//MEAN_adc(1,0,num);
			//MEAN_adc(2,0,num);
		/**/
		RtPrintf("FDBACK[1,2,3] = %5u;\t\t%5u;\t\t%5u;\n",
			MEAN_adc(0,0,num),
			MEAN_adc(1,0,num),
			MEAN_adc(2,0,num)
			);
		//*/
/*
		data = GET_ADC(0);
		RtPrintf("FDBACK1=%5u\t",data);
		data = GET_ADC(1);
		RtPrintf("FDBACK2=%5u\t",data);
		data = GET_ADC(2);

		RtPrintf("FDBACK3=%5u\n",data);	
*/

		RtSleep(100);
	}
	

    ExitProcess(0);
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// GET_ADC
DWORD GET_ADC(UCHAR channel, UCHAR gain)
{
	UCHAR ready;	
	UCHAR mp;
	UCHAR ADlow;								
	UCHAR ADhigh;								
	DWORD val;									
	unsigned short c;

	// set gain and mux
	// x|x|MUX3|MUX2|MUX1|MUX0|GAIN1|GAIN0
	// MUX[3-0] = binary number selecting from 0to15 shifted left 2 
	//	0b0000 0000<<2 = 0x00<<2 = 0x00 = 0<<2 = AI0
	//	0b0000 0001<<2 = 0x01<<2 = 0x04 = 1<<2 = AI1
	//	0b0000 0010<<2 = 0x02<<2 = 0x08 = 2<<2 = AI2
	//	0b0000 0100<<2 = 0x03<<2 = 0x0C = 3<<2 = AI3
	// etc
	gain = 0;
	mp = channel<<2 | gain;

	RtWritePortUchar((PUCHAR)(baseAddress+AD_GAIN), mp);

	RtSleep(1);

	RtWritePortUchar((PUCHAR)(baseAddress+AD_SOFT_TRIGGER_START), 0x01);

	for ( c = 0, ready = 0x00 ; (ready == 0x00) && (c <= 10) ; c++ ) {
		ready = RtReadPortUchar((PUCHAR)(baseAddress+AD_STATUS));		
		ready = ready & 0x01;
	}

	if ( c > 10 ) {
		RtPrintf("GetADC: Reading timeout\n");
		return 0;		
	}

	ADlow = RtReadPortUchar((PUCHAR)(baseAddress+AD_LOWBYTE));
	ADhigh = RtReadPortUchar((PUCHAR)(baseAddress+AD_HIGHBYTE));
	val = ADhigh;
	val = val << 8;
	val = val | ADlow;
	/*
	RtPrintf("control byte = 0x%08x;\t\tADlow = %i;\t\tADhigh = %i;\t\tval = %lu\n",
		channel, ADlow, ADhigh, val);
	*/	
	return val;
}

