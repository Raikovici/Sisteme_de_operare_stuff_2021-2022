/**
 * SO, 2017
 * Lab #3, Procese
 *
 * Task #2, Windows
 *
 * Implementing a minimal shell
 */

/* do not use UNICODE */
#undef _UNICODE
#undef UNICODE

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"

#define MAX_LINE_SIZE	256
#define MAX_CMD_SIZE	32
#define MAX_ARGS		8

#define	ERROR			0
#define SIMPLE			1
#define PIPE			2
#define SET_VAR			3
#define EXIT_OK			4

static PCHAR command1, command2;
static PCSTR stdinFilename, stdoutFilename, stderrFilename;
static PCHAR value, var;
static CHAR varBuf[32];
static INT type;

static INT ParseLine(PCHAR line);
static INT ParseSimpleCommand(PCHAR line, PCHAR *command);
static VOID AllocateMemory(VOID);
static VOID Cleanup(VOID);
static HANDLE MyOpenFile(PCSTR filename);


/*
 * @psi		- STATRTUPINFO of the child process
 * @hFile	- file handle for redirect
 * @opt		- redirect option is one of the following
 *		 STD_INPUT_HANDLE,STD_OUTPUT_HANDLE, STD_ERROR_HANDLE
 */
static VOID RedirectHandle(STARTUPINFO *psi, HANDLE hFile, INT opt)
{
	if (hFile == INVALID_HANDLE_VALUE)
		return;

	/* TODO 1 - set handles from psi to
	 * current STDIN,STDOUT, STDERR handles
	 */
	psi->cb = sizeof(psi);
	psi->hStdInput = GetStdHandle(STD_INPUT_HANDLE); //iau standard input
	psi->hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE); //iau standard output
	psi->hStdError = GetStdHandle(STD_ERROR_HANDLE); //iau standard error

	psi->dwFlags |= STARTF_USESTDHANDLES; // IMPORTANT -> nu merge mostenirea fara asta
	/* TODO 1 - Redirect one of STDIN, STDOUT, STDERR to hFile */
	switch (opt) {
	case STD_INPUT_HANDLE:
		/* TODO 1 */
		psi->hStdInput = hFile; //redirectez inputu
		break;
	case STD_OUTPUT_HANDLE:
		/* TODO 1 */
		psi->hStdOutput = hFile; //redirectez outputu
		break;
	case STD_ERROR_HANDLE:
		/* TODO 1 */
		psi->hStdError = hFile; //redirectez erroru
		break;   
	}
}

static PCHAR ExpandVariable(PCSTR key)
{
	DWORD dwRet;

	memset(varBuf, 0, MAX_CMD_SIZE);
	dwRet = GetEnvironmentVariable(key, varBuf, sizeof(varBuf));
	DIE(!dwRet, "GetEnvironmentVariable");

	return varBuf;
}

static VOID CloseProcess(LPPROCESS_INFORMATION lppi)
{
	CloseHandle(lppi->hThread);
	CloseHandle(lppi->hProcess);
}

static VOID RunSimpleCommand(PCHAR command)
{
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	DWORD dwRet;
	BOOL bRet;
	HANDLE hFile;

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	/* Call my redirect function */
	if (stdoutFilename != NULL) {
		hFile = MyOpenFile(stdoutFilename);
		RedirectHandle(&si, hFile, STD_OUTPUT_HANDLE); //redirectez handle ul catre hFile
	}

	bRet = CreateProcess(
			 NULL,          //nicio aplicatie
			 command,       //comanda care va fi executata de process
			 NULL,          // nu i proces mostenit
			 NULL,          // n are threaduri mostenite
			 TRUE,         //seteaza daca poate avea mostenitori
			 0,             //n are flaguri speciale
			 NULL,          // nu i mostenit, n are ce sa foloseasca de la parinte
			 NULL,          
			 &si,           // pointer la o structura STARTUPINFO
			 &pi);          // pointer la PROCESS_INFORMATION
	DIE(bRet == FALSE, "Create Process");


	dwRet = WaitForSingleObject(pi.hProcess, INFINITE); //astept sa se incheie
	DIE(dwRet == WAIT_FAILED, "WaitForSingleObject");

	CloseProcess(&pi); //il inchid

	if (stdoutFilename != NULL) {
		bRet = CloseHandle(hFile); //inchid handle ul
		DIE(bRet == FALSE, "CloseHandle");
	}
}

/*
 * Purpouse: launch a command like' command1 | command2 '
 *   @command1 - first command string
 *   @command2 - second command string
 */
static VOID PipeCommands(PCHAR command1, PCHAR command2)
{
	SECURITY_ATTRIBUTES sa;
	HANDLE hRead, hWrite; /* Pipe handles */

	STARTUPINFO si1, si2;
	PROCESS_INFORMATION pi1, pi2;
	DWORD dwRet;
	BOOL bRet;

	/* TODO 2 - Init security attributes and process info */
	ZeroMemory(&sa, sizeof(sa)); //initializez str de sec atr
	sa.bInheritHandle = TRUE; //tre sa fie inheritable, sa mosteneasca tabela de descriptori a paritnelui
	ZeroMemory(&pi1, sizeof(pi1));
	ZeroMemory(&pi2, sizeof(pi2));
	/* TODO 2 - Create pipe */
	ZeroMemory(&si1, sizeof(si1));
	ZeroMemory(&si2, sizeof(si2));
	si1.cb = sizeof(si1);
	si2.cb = sizeof(si2);
	si1.dwFlags |= STARTF_USESTDHANDLES; //flagu asta imi zice ca se folosesc alte handle uri pt input, output si error 
	si2.dwFlags |= STARTF_USESTDHANDLES; //decat cele implicite (vor fi trecuta prin RedirectHandle()
	bRet = CreatePipe( 
		&hRead, //capatu de citire
		&hWrite, //capatu de scriere
		&sa, //sec atrr
		0
	);
	DIE(bRet == FALSE, "cannot create pipe");
	
	/*
	 * TODO 2 - Launch command1 with stdout redirected using CreateProcess
	 *        - This child process should write to pipe
	 */
	RedirectHandle(&si1, hWrite, STD_OUTPUT_HANDLE); //redirectez outputul capatului de scriere
													 //el nu va mai fi reprezentat de outputu normal, ci de hWrite, adica 
													 //folosesc capatu de scriere al pipe ului 
	bRet = CreateProcess( //creez proces pt prima comandda
		NULL,
		command1,
		NULL,
		NULL,
		TRUE,
		0,
		NULL,
		NULL,
		&si1,
		&pi1
	);
	DIE(bRet == FALSE, "cannot create process 1");
	CloseHandle(hWrite);
	RedirectHandle(&si2, hRead, STD_INPUT_HANDLE); //redirectez inputul capatului de citire
													 //el nu va mai fi reprezentat de inputu normal, ci de hRead, adica 
													 //folosesc capatu de citire al pipe ului sa citesc ce am scris in prealabil
	/*
	 * TODO 2 - Launch command2 with stdin redirected using CreateProcess
	 *        - This child process should read from the pipe
	 */
	bRet = CreateProcess( //creez procesu
		NULL,
		command2,
		NULL,
		NULL,
		TRUE,
		0,
		NULL,
		NULL,
		&si2,
		&pi2
	);
	DIE(bRet == FALSE, "cannot create process 2");
	CloseHandle(hRead);
	/* TODO 2 - Wait for processes to finish */
	dwRet = WaitForSingleObject(pi1.hProcess, INFINITE); //astept dupa primu sa scrie in pipe
	DIE(dwRet == WAIT_FAILED, "cannot wait");
	CloseProcess(&pi1);
	dwRet = WaitForSingleObject(pi2.hProcess, INFINITE); //astept dupa al doilea sa citeasca din pipe
	DIE(dwRet == WAIT_FAILED, "cannot wait");
	CloseProcess(&pi2);

}

int main(int argc, char **argv)
{
	CHAR line[MAX_LINE_SIZE];

	AllocateMemory();

	while (1) {
		printf("> ");
		fflush(stdout);

		ZeroMemory(line, MAX_LINE_SIZE);

		if (fgets(line, sizeof(line), stdin) == NULL)
			exit(EXIT_SUCCESS);

		type = ParseLine(line);

		switch (type) {
		case EXIT_OK: 
			exit(EXIT_SUCCESS);

		case SET_VAR:
			SetEnvironmentVariable(var, value);
			break;

		case PIPE:
			PipeCommands(command1, command2);
			break;

		case SIMPLE:
			RunSimpleCommand(command1);
			break;
		}
	}

	Cleanup();
	return 0;
}

static VOID AllocateMemory(VOID)
{
	command1 = malloc(256 * sizeof(char));
	command2 = malloc(256 * sizeof(char));
}

VOID Cleanup(VOID)
{
	free(command1);
	free(command2);
}

static HANDLE MyOpenFile(PCSTR filename)
{
	SECURITY_ATTRIBUTES sa;

	ZeroMemory(&sa, sizeof(sa));
	sa.bInheritHandle = TRUE;

	return CreateFile(
		filename,
		GENERIC_WRITE,
		FILE_SHARE_READ,
		&sa,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
}

static INT ParseLine(PCHAR line)
{
	INT ret = SIMPLE;
	PCHAR pos;

	stdinFilename = NULL;
	stdoutFilename = NULL;
	stderrFilename = NULL;

	ZeroMemory(command1, MAX_CMD_SIZE);
	ZeroMemory(command2, MAX_CMD_SIZE);

	/* comanda vida */
	if (strcmp(line, "\n") == 0)
		return ERROR;

	/* comanda exit */
	if (strncmp("exit", line, strlen("exit")) == 0)
		return EXIT_OK;

	/* var = value */
	if (strchr(line, '=') != 0) {
		PCHAR delim = "=\n";
		PCHAR nextToken;
		PCHAR token = strtok_s(line, delim, &nextToken);

		if (token == NULL)
			return 0;

		var = _strdup(token);

		token = strtok_s(NULL, delim, &nextToken);
		if (token == NULL)
			return 0;

		value = _strdup(token);
		return SET_VAR;
	}

	/* verificare de comanda pipe */
	pos = strchr(line, '|');
	if (pos != 0) {
		strncat_s(command1, MAX_CMD_SIZE, line,
				strlen(line) - strlen(pos));

		/* scapam de | */
		pos++;
		ParseSimpleCommand(pos, &command2);

		return PIPE;
	}

	ParseSimpleCommand(line, &command1);

	return SIMPLE;
}

static INT ParseSimpleCommand(PCHAR line, PCHAR *command)
{
	PCSTR delim = " \t\n";
	PCHAR nextToken;
	PCHAR token = strtok_s(line, delim, &nextToken);

	if (token == NULL)
		return ERROR;

	/* parcurgere argumente + expandare + verificare de redirectari */
	while (token != NULL) {
		if (token[0] == '$') {
			token = ExpandVariable(token+1);

			if (token == NULL) {
				printf(" Expansion failed\n");
				return ERROR;
			}
		}

		if (strncmp(token, ">", strlen(">")) == 0) {
			if (strlen(token) > strlen(">")) {
				token++;
				stdoutFilename = _strdup(token);
			} else {
				token = strtok_s(NULL, delim, &nextToken);
				stdoutFilename = _strdup(token);
			}
		} else {
			strcat_s(*command, MAX_CMD_SIZE, token);
			strcat_s(*command, MAX_CMD_SIZE, " ");
		}

		token = strtok_s(NULL, delim, &nextToken);
	}

	return SIMPLE;
}
