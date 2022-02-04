/* do not use UNICODE */
#undef _UNICODE
#undef UNICODE

#include <windows.h>
#include <stdio.h>

#include "utils.h"

#define NO_THREADS 50

/* function executed by the threads */
DWORD WINAPI ThreadFunc(LPVOID lpParameter)
{
	printf("thread %d starting\n", lpParameter);

	/* do some work */
	while (1) {
		/* do some work */
		Sleep(1000);
	}

	return 0;
}

DWORD main(VOID)
{
	DWORD IdThread, dwReturn;
	HANDLE hThread[NO_THREADS];
	int i;

	setbuf(stdout, NULL);

	/* create threads */
	for (i = 0; i < NO_THREADS; i++) {
		hThread[i] = CreateThread(
			NULL, //sec attr
			0,    //dimensiunea standard a stivei threadurlui
			(LPTHREAD_START_ROUTINE)ThreadFunc, //functia executata de thread
			(LPVOID)i, //n are parametru
			0,          //niciun flag special
			&IdThread); //id ul threadului
		DIE(hThread[i] == NULL, "CreateThread");
	}

	/* wait for threads completion */
	for (i = 0; i < NO_THREADS; i++) {
		dwReturn = WaitForSingleObject(hThread[i], INFINITE); //astept pt fiecare thread sa si incheie executia
		DIE(dwReturn == WAIT_FAILED, "WaitForSingleObject");
	}

	return 0;
}
