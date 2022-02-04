#include <stdlib.h>
#include <stdio.h>
#include "so_stdio.h"
#include "utils.h"
#include <string.h>
#include <Windows.h>
#define INVALID_FLAG -1
#define BUFFER_SIZE 4096
#define DLL_EXPORTS

struct _so_file {
	HANDLE _handle; // handle ul fisierului
	char _openingMode[5]; //modul de deschidere al fisierului
	int _endOfFile; // daca am ajuns la end of file
	char _buffer[BUFFER_SIZE]; // bufferul in care scriem sau din care citim 
	char _lastOperation[1024]; //numele ultimei operatii 
	int _actualPosInBuffer; //pozitia actuala in buffer
	DWORD _actualPosInFile; //dimensiunea actuala a bufferului
	int _errorFile; //daca intalnesc eroare
	DWORD _actualBufferSize; //cati bytes am in buffer
	PROCESS_INFORMATION _childStructure; // pid ul procesului copil
};

DWORD getAccessMode(const char* mode) {
	if(isRead(mode) == 1) {
		return GENERIC_READ;
	}
	else if((isWrite(mode) == 1) || (isAppend(mode) == 1)) {
		return GENERIC_WRITE;
	}
	else if((isAppendRead(mode) == 1) || (isWriteRead(mode) == 1)
		|| (isReadWrite(mode) == 1)) {
		return GENERIC_READ | GENERIC_WRITE;
	}
	else {
		return -1;
	}
}

DWORD getAction(const char* mode) {
	if((isRead(mode) == 1) || (isReadWrite(mode) == 1)) {  //r, r+
		return OPEN_EXISTING;
	}
	else if((isWrite(mode) == 1) || (isWriteRead(mode) == 1)) { //w, w+
		return CREATE_ALWAYS;
	}
	else if((isAppend(mode) == 1) || (isAppendRead(mode) == 1)) { //a, a+
		return OPEN_ALWAYS; 
	}
	else {
		return -1;
	}
}

SO_FILE *so_fopen(const char *pathname, const char *mode) {
	SO_FILE *f;
	HANDLE hHandle;
	DWORD openingMode, openingAction;
	
	openingMode = getAccessMode(mode);
	if(openingMode == -1) {
		printf("Cannot assign opening flags\n");
		return NULL;
	}
	openingAction = getAction(mode);
	if(openingAction == -1) {
		printf("Cannot assign opening action\n");
		return NULL;
	}

	hHandle = CreateFile(
						(LPCSTR)pathname, /* filename*/
						GENERIC_READ | GENERIC_WRITE, /* access mode */
						FILE_SHARE_READ | FILE_SHARE_WRITE, /* sharing option */
						NULL, /* security attributes */
						openingAction, /* open action*/
						FILE_ATTRIBUTE_NORMAL, /* file attributes */
						NULL /*no template*/
					);
	if(hHandle == INVALID_HANDLE_VALUE) {
		printf("Cannot assign a valid handle\n");
		return NULL;
	}
	f = (SO_FILE*)malloc(sizeof(SO_FILE));
	if(f == NULL) {
		printf("Cannot allocate memory\n");
		f->_errorFile = 1;
		free(f);
		return NULL;
	}
	
	strcpy(f->_openingMode, mode);
	strcpy(f->_buffer, "");
	strcpy(f->_lastOperation, "");
	f->_handle = hHandle;
	f->_actualPosInFile = 0;
	f->_endOfFile = 0;
	f->_actualPosInBuffer = 0;
	f->_errorFile = 0;
	f->_actualBufferSize = 0;
	f->_childStructure.hProcess = INVALID_HANDLE_VALUE;
	f->_childStructure.hThread = INVALID_HANDLE_VALUE;
	return f;
}

int so_fclose(SO_FILE *stream) {
	int ok;
	int bRet;
	if(isWriteOperation(stream->_lastOperation) == 1) {
		ok = so_fflush(stream);
	}

	bRet = CloseHandle(stream->_handle);
	if(bRet < 0) {
		stream->_errorFile = 1;
		if(stream != NULL) {
			stream->_handle = INVALID_HANDLE_VALUE;
			free(stream);
			stream = NULL;
		}
	}
	if(stream != NULL) {
		free(stream);
		stream = NULL;
	}

	return 0;
}

HANDLE so_fileno(SO_FILE *stream) {
	return stream->_handle;
}

long so_ftell(SO_FILE *stream) {
	return stream->_actualPosInFile;
}

int so_fflush(SO_FILE *stream) {
	int bRet;
	DWORD bytesWritten;
	if(isWriteOperation(stream->_lastOperation) == 1) {
		stream->_actualPosInBuffer = 0;
		while(stream->_actualBufferSize > 0) {
			bRet = WriteFile (
								stream->_handle, /* open file handle */
								&stream->_buffer[stream->_actualPosInBuffer],  /* start of data to write */
								stream->_actualBufferSize, /* number of bytes to write */
								&bytesWritten, /* number of bytes that were written */
								NULL  /* no overlapped structure */
							);
			if(bRet == -1) {
				stream->_errorFile = -1;
				return SO_EOF;
			}
			stream->_actualPosInBuffer += bytesWritten;
			stream->_actualBufferSize -= bytesWritten;
			
		}
		stream->_actualPosInBuffer = 0;
		return 0;
	}
	else {
		return SO_EOF;
	}
}

int so_fseek(SO_FILE *stream, long offset, int whence) {

	int bRet;
	int position;
	if(isWriteOperation(stream->_lastOperation) == 1) {
		bRet = so_fflush(stream);
		if(bRet == SO_EOF) {
			return SO_EOF;
		}
	}
	else if(isReadOperation(stream->_lastOperation) == 1) {
		stream->_actualPosInBuffer = 0;
		stream->_actualBufferSize = 0;
		strcpy(stream->_buffer, "");
	}
	strcpy(stream->_lastOperation, "");
	if(whence == SEEK_CUR) {
		offset += stream->_actualPosInFile;
		whence = SEEK_SET;
	}
	
	position = SetFilePointer(
								  stream->_handle, /*handle file */
								  offset, /* offset*/
								  NULL, /* no 64bytes offset */
								  (DWORD)whence /* the actual whence*/
							  );
	if(position == INVALID_SET_FILE_POINTER) {
		stream->_errorFile = 1;
		return SO_EOF;
	}
	stream->_actualPosInFile = position;
	return 0;
	
}

size_t so_fread(void *ptr, size_t size, size_t nmemb, SO_FILE *stream) {

	int i;
	int c;
	i = 0;

	if(canRead(stream->_openingMode) == 1) {
		strcpy(stream->_lastOperation, "read_bunch_of_bytes");
		while( i < nmemb) {
			c = so_fgetc(stream);
			if(c == SO_EOF){
				stream->_errorFile = 1;
				return SO_EOF;
			}
			*((char*)ptr)++ = (char)c;
			i++;
		}
		return nmemb;
	}
	else {
		stream->_errorFile = 1;
		return SO_EOF;
	}
}

size_t so_fwrite(const void *ptr, size_t size, size_t nmemb, SO_FILE *stream) {

	int i;
	int bRet;
	char c;
	i = 0;
	if(canWrite(stream->_openingMode) == 1) {
		strcpy(stream->_lastOperation, "write_bunch_of_bytes");
		while(i < nmemb) {
			c = so_fgetc(*((char*)ptr)++, stream);
			if(c == SO_EOF) {
				stream->_errorFile = 1;
				return SO_EOF;
			}
			i++;
		}
		return nmemb;
	}
	else {
		stream->_errorFile = 1;
		return SO_EOF;
	}
}

int so_fgetc(SO_FILE *stream) {

	int bRet;
	DWORD bytesRead;
	if(canRead(stream->_openingMode) == 1)
	{
		if(isWriteOperation(stream->_lastOperation) == 1) {
			bRet = so_fflush(stream);
			if (bRet == -1) {
				stream->_errorFile = 1;
				return SO_EOF;
			}
			
		}
		if(stream->_actualBufferSize > 0) {
			--stream->_actualBufferSize;
			++stream->_actualPosInFile;
			strcpy(stream->_lastOperation, "read_a_char");
			return (int)stream->_buffer[stream->_actualPosInBuffer++];
		}
		else {
			stream->_actualPosInBuffer = 0;
			bRet = ReadFile(
								 stream->_handle,  /* open file handle */
								 &stream->_buffer[stream->_actualPosInBuffer],  /* where to put data */
								 BUFFER_SIZE, /* number of bytes to read */
								 &bytesRead, /* number of bytes that were read */
								 NULL /* no overlapped structure */
							   );
							   
			if(bRet == -1) {
				stream->_endOfFile = 1;
				return SO_EOF;
			}
			
			if(bytesRead == 0) {
				stream->_endOfFile = -1;
				return SO_EOF;
			}
			else {
				stream->_actualBufferSize += bytesRead;
				++stream->_actualPosInFile;
				--stream->_actualBufferSize;
				strcpy(stream->_lastOperation, "read_a_char");
				return (int)stream->_buffer[stream->_actualPosInBuffer++];
			}
		}
	}
	else {
		stream->_errorFile = 1;
		return SO_EOF;
	}
}

int so_fputc(int c, SO_FILE *stream) {
	DWORD position;
	int whence;
	int seek;
	int offset;
	int bRet;
	if(canWrite(stream->_openingMode) == 1) {
		if(isAppend(stream->_openingMode) == 1) {
			position = SetFilePointer(
								  stream->_handle, /*handle file */
								  0, /* offset*/
								  NULL, /* no 64bytes offset */
								  (DWORD)SEEK_END /* the actual whence*/
								);
			if(position == -1) {
				stream->_errorFile = 1;
				return SO_EOF;
			}
		}
		
		if(isReadOperation(stream->_lastOperation) == 1)
		{
			offset = stream->_actualPosInFile;
			whence = SEEK_SET;
			
			if(isAppendRead(stream->_openingMode) == 1) {
				offset = 0;
				whence = SEEK_END;
			}
			seek = 1;
		}
		
		if((isAppendRead(stream->_openingMode) == 1) && (strcmp(stream->_lastOperation, "") == 0)) {
			offset = 0;
			whence = SEEK_END;
			seek = 1;
		}
		if(seek == 1) {
			position = so_fseek(stream, offset, whence);
			if(position == -1) {
				stream->_errorFile = 1;
				return SO_EOF;
			}
		}
		if(stream->_actualPosInBuffer < BUFFER_SIZE) {
			++stream->_actualPosInFile;
			stream->_buffer[stream->_actualPosInBuffer++] = (char)c;
			++stream->_actualBufferSize;
			strcpy(stream->_lastOperation, "write_a_char");
			return c;
		}
		bRet = so_fflush(stream);
		if(bRet == -1) {
			stream->_errorFile = 1;
			return SO_EOF;
		}
		++stream->_actualPosInFile;
		stream->_buffer[stream->_actualPosInBuffer++] = (char)c;
		++stream->_actualBufferSize;
		strcpy(stream->_lastOperation, "write_a_char");
		return c;
	}
	else {
		stream->_errorFile = 1;
		return SO_EOF;
	}
}

int so_feof(SO_FILE *stream) {
	return stream->_endOfFile;
}

int so_ferror(SO_FILE *stream) {
	return stream->_errorFile;
}

void redirectHandle(STARTUPINFO *psi, HANDLE handle, int opt) {
	if(handle == INVALID_HANDLE_VALUE) {
		return;
	}
	
	psi->hStdInput = GetStdHandle(STD_INPUT_HANDLE);
	psi->hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	psi->hStdError = GetStdHandle(STD_ERROR_HANDLE);
	
	psi->dwFlags |= STARTF_USESTDHANDLES;
	
	if(opt == STD_INPUT_HANDLE) {
		psi->hStdInput = handle;
	}
	else if(opt == STD_OUTPUT_HANDLE) {
		psi->hStdOutput = handle;
	}
	else {
		psi->hStdError = handle;
	}
}

SO_FILE *so_popen(const char *command, const char *type) {


	SECURITY_ATTRIBUTES sa;
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	HANDLE hRead, hWrite;
	int bRet;
	char commandExec[1024];
	HANDLE handleParent, handleChild;
	SO_FILE* f;
	
	ZeroMemory(&sa, sizeof(sa));
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.bInheritHandle = TRUE;
	
	
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	
	ZeroMemory(&pi, sizeof(pi));
		
	bRet = CreatePipe(
						&hRead,   /*capatul de citire */
						&hWrite, /* capatul de scriere */
						&sa, /* pentru moștenire sa.bInheritHandle=TRUE */
						0  /*dimensiunea default pentru pipe */
					);
	if(bRet == -1) {
		return NULL;
	}
	
	if(isRead(type) == 1) {
		handleParent = hRead;
		handleChild = hWrite;
		redirectHandle(&si, hWrite, STD_OUTPUT_HANDLE);
	}
	else if(isWrite(type) == 1) {
		handleParent = hWrite;
		handleChild = hRead;
		redirectHandle(&si, hRead, STD_INPUT_HANDLE);
	}
	SetHandleInformation(handleParent, HANDLE_FLAG_INHERIT, 0);   
	
	strcpy(commandExec, command);	
	
	bRet = CreateProcess (
						NULL, /* No module name (use command line) */
						(LPCSTR)commandExec,  /* Command line */
						NULL,  /* Process handle not inheritable */
						NULL,  /* Thread handle not inheritable */
						TRUE,  /* Set handle inheritance to TRUE */
						0,  /* No creation flags */
						NULL, /* Use parent's environment block */
						NULL, /* Use parent's starting directory */ 
						&si, /* Pointer to STARTUPINFO structure */
						&pi  /* Pointer to PROCESS_INFORMATION structure */
					);
	if(bRet == -1) {
		CloseHandle(hRead);
		CloseHandle(hWrite);
		return NULL;
	}
	CloseHandle(handleChild);
	
	f = (SO_FILE*)malloc(sizeof(SO_FILE));
	if(f == NULL) {
		printf("Cannot allocate memory\n");
		f->_errorFile = 1;
		CloseHandle(handleParent);
		free(f);
		return NULL;
	}
	

	f->_handle = handleParent;
	strcpy(f->_openingMode, type);
	strcpy(f->_buffer, "");
	strcpy(f->_lastOperation, "");
	f->_actualPosInFile = 0;
	f->_endOfFile = 0;
	f->_actualPosInBuffer = 0;
	f->_errorFile = 0;
	f->_actualBufferSize = 0;
	f->_childStructure.hProcess = pi.hProcess;
	f->_childStructure.hThread = pi.hThread;
	return f;
}

int so_pclose(SO_FILE *stream) {

	PROCESS_INFORMATION processStructure;
	int ok;
	int status;
	
	if((stream->_childStructure.hProcess == INVALID_HANDLE_VALUE) || 
		(stream->_childStructure.hThread == INVALID_HANDLE_VALUE)){
		return SO_EOF;
	}
	else {
		processStructure = stream->_childStructure;
		ok = so_fclose(stream);
		if(ok < 0) {
			return SO_EOF;
		}
		ok = WaitForSingleObject(processStructure.hProcess, INFINITE);
		if(ok == WAIT_FAILED) {
			return SO_EOF;
		}
		status = GetExitCodeProcess(processStructure.hProcess, &ok);
		CloseHandle(processStructure.hProcess);
		CloseHandle(processStructure.hThread);
		return ok;
	}
}
