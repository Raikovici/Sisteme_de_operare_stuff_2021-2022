/**
 * SO, 2016 - Lab #4, Signals
 * Task #1, Windows
 *
 * Timer usage.
 */

#define _WIN32_WINNT	0x0500

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include "utils.h"
#include <windows.h>

#define _SECOND		10000000
#define TIMEOUT		(-2 * _SECOND)


/*
 * Timer APC routine
 */
static VOID CALLBACK TimerAPCProc(LPVOID lpArgToCompletionRoutine,
		DWORD dwTimerLowValue, DWORD dwTimerHighValue)
{
	/* TODO: Use ctime and time to print current time. */
	time_t currentTime; //timpul curent
	currentTime = time(NULL); //initializez folosind functia time
	printf("The time is %s\n", ctime(&currentTime)); //printez timpul curent in format normal
	
}

/*
 * Initialise and configure timer
 */
static void InitSetTimer(void)
{
	/*
	 * TODO
	 * First, create your timer using CreateWaitableTimer.
	 * Then set its timeout and its routine using SetWaitableTimer.
	 * No data needs to be passeed to the completion routine.
	 * Don't forget to check those calls for errors.
	 */
	HANDLE hTimer;
	LARGE_INTEGER liDueTime;
	BOOL bRet;
	
	hTimer = CreateWaitableTimer(NULL, TRUE, NULL); //creez timerul
	DIE(hTimer == NULL, "cannot create");
	
	liDueTime.QuadPart = TIMEOUT;
	bRet = SetWaitableTimer( //il armez sa se execute o data la 2000 ms
		hTimer,
		&liDueTime,
		2000,
		TimerAPCProc,
		NULL,
		FALSE
	);
	DIE(bRet == FALSE, "cannot set timer");
}

int main(void)
{
	InitSetTimer(); /* configures the timer to 2 seconds */

	while (1) {
		/* TODO: Wait for timer (use SleepEx function). */
		SleepEx(INFINITE, TRUE);
	}

	return 0;
}
