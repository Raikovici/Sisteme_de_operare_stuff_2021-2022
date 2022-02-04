/* do not use UNICODE */
#undef _UNICODE
#undef UNICODE

#define _WIN32_WINNT    0x500
#include <windows.h>
#include <stdio.h>

#include "utils.h"

#define PERIOD       1000
#define TIMES        3

HANDLE finished;

VOID CALLBACK TimerFunction(PVOID lpParam, BOOLEAN TimerOrWaitFired)
{
	static int count;
	BOOL bRet;

	printf("'TimerFunction' has been called and count is %d\n", count);

	/* TODO - Check if we must increment counter or finish */
	if (count == TIMES - 1) {
		bRet = ReleaseSemaphore(finished, 1, NULL);
		DIE(bRet == FALSE, "release semaphore");
	}
	else {
		count++;
	}
}

int main(void)
{
	HANDLE timer_queue;
	HANDLE timer;
	BOOL bRet;
	DWORD dwRet;

	setbuf(stdout, NULL);

	/* TODO - Create a TimerQueue */
	timer_queue = CreateTimerQueue();
	DIE(timer_queue == INVALID_HANDLE_VALUE, "cannot create timer queue");
	/* TODO - Create a semaphore/event */
	finished = CreateSemaphore(
		NULL,
		0,
		1,
		NULL
	);
	DIE(finished == INVALID_HANDLE_VALUE, "create semaphore");
	/* TODO - Create a timer and associate it with the timer queue */
	bRet = CreateTimerQueueTimer(
		&timer, //handle ul care va fi tine minte timerul creat
		timer_queue, //apartine acestei cozi
		TimerFunction, //functia executata de timer
		NULL, //n are niciun parametru
		0, //dupa cat timp va expira
		PERIOD, //perioada timerului
		0 //niciun flag spercial
	);
	DIE(bRet == FALSE, "cannot create timer for timer queue");
	/* TODO - Wait for the semaphore/event to be set so we can free resources */
	dwRet = WaitForSingleObject(finished, INFINITE);
	DIE(dwRet == WAIT_FAILED, "cannot wait");
	/* TODO - Delete the timer queue and its timers */
	bRet = DeleteTimerQueue(timer_queue);
	DIE(bRet == FALSE, "delete timer queue");
	return 0;
}
