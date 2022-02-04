/*
 * SO, 2016 - Lab 10 (Advanced I/O Windows
 * I/O completion ports wrapper functions
 * Task #4 (I/O completion ports)
 */

#include <windows.h>

#include "iocp.h"

/*
 * create I/O completion port
 */

HANDLE iocp_init(void)
{
	/* TODO */
	return CreateIoCompletionPort( //creez completion portul
		INVALID_HANDLE_VALUE, //important, dau aceasta valoare fiindca il creez pt prima oara
		NULL,
		NULL,
		0
	);
}

/*
 * add handle to completion port; use handle as key
 */

HANDLE iocp_add(HANDLE iocp, HANDLE hFile)
{
	/* TODO */
	return CreateIoCompletionPort( //adaug handle ul la completion port
		hFile,
		iocp,
		(ULONG_PTR)hFile,
		0
	);
}

/*
 * add handle to completion port; use key as key
 */

HANDLE iocp_add_key(HANDLE iocp, HANDLE hFile, ULONG_PTR key)
{
	/* TODO */
	return CreateIoCompletionPort( //adaug si cheia 
		hFile,
		iocp,
		key,
		0
	);
}

/*
 * wait for event notification on completion port
 */

BOOL iocp_wait(HANDLE iocp, PDWORD bytes, PULONG_PTR key, LPOVERLAPPED *op)
{
	
	/* TODO */
	BOOL bRet;
	bRet = GetQueuedCompletionStatus( //astept incheierea operatiei asincrone
		iocp,
		&bytes,
		&key,
		&op,
		INFINITE
	);
	return bRet;
}
