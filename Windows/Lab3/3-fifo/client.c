/**
 * SO, 2017
 * Lab #3, Procese
 *
 * Task #3, Windows
 *
 * Named Pipes - Client side
 */

/* do not use UNICODE */
#undef _UNICODE
#undef UNICODE

#include <windows.h>
#include <stdio.h>
#include <string.h>

#include "utils.h"
#include "common.h"


int main(void)
{
	HANDLE hPipe;
	char *message[BUFSIZE];
	DWORD dwBytesWritten, dwTotalBytesWritten = 0, dwMsgLen;
	BOOL bRet;

	/* TODO - Open Named Pipe as a regular file */
	hPipe = CreateFile(
		PIPE_NAME,
		GENERIC_WRITE,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL
	);
	DIE(hPipe == INVALID_HANDLE_VALUE, "cannot open pipe");
	/* Read mesage from user */
	printf("Message to send:");
	ZeroMemory(message, sizeof(message));
	scanf_s("%[^\n]", message, sizeof(message));
	dwMsgLen = strlen((const char *)message);

	/* TODO - Send the message */
	do {
		bRet = WriteFile(
			hPipe,
			message + dwTotalBytesWritten,
			dwMsgLen - dwTotalBytesWritten,
			&dwBytesWritten,
			NULL
		);
		DIE(bRet == FALSE, "cannot write to pipe");
		dwTotalBytesWritten += dwBytesWritten;
	} while (dwTotalBytesWritten < dwMsgLen);
	/* Closing our end of the pipe */
	bRet = FlushFileBuffers(hPipe);
	DIE(bRet == FALSE, "FlushFileBuffers");

	bRet = CloseHandle(hPipe);
	DIE(bRet == FALSE, "CloseHandle");

	return 0;
}
