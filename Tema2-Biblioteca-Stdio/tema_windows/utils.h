#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <windows.h>
struct _so_file;
typedef struct _so_file SO_FILE;

int isWriteOperation(char* string);
int isReadOperation(char* string);

int isRead(const char* mode);
int isReadWrite(const char* mode);
int isWrite(const char* mode);
int isWriteRead(const char* mode);
int isAppend(const char* mode);
int isAppendRead(const char* mode);

DWORD getAccessMode(const char* mode);
DWORD getAction(const char* mode);

int canRead(char* mode);
int canWrite(char* mode);

void redirectHandle(STARTUPINFO *psi, HANDLE h_file, int opt);
