#include "utils.h"

int isWriteOperation(char* string) {
	if(strstr(string, "write") != NULL) {
		return 1;
	}
	else {
		return 0;
	}
}

int isReadOperation(char* string) {
	if(strstr(string, "read") != NULL) {
		return 1;
	}
	else {
		return 0;
	}
}

int isRead(const char* mode) {
	if(strcmp(mode, "r") == 0) {
		return 1;
	}
	else {
		return 0;
	}
}

int isReadWrite(const char* mode) {
	if(strcmp(mode, "r+") == 0) {
		return 1;
	}
	else {
		return 0;
	}
}

int isWrite(const char* mode) {
	if(strcmp(mode, "w") == 0) {
		return 1;
	}
	else {
		return 0;
	}
}

int isWriteRead(const char* mode) {
	if(strcmp(mode, "w+") == 0) {
		return 1;
	}
	else {
		return 0;
	}
}

int isAppend(const char* mode) {
	if(strcmp(mode, "a") == 0) {
		return 1;
	}
	else {
		return 0;
	}
}

int isAppendRead(const char* mode) {
	if(strcmp(mode, "a+") == 0) {
		return 1;
	}
	else {
		return 0;
	}
}

int canRead(char* mode) {
	if ((strcmp(mode, "r") == 0) || (strcmp(mode, "a+") == 0)
		|| (strcmp(mode, "r+") == 0) || (strcmp(mode, "w+") == 0)) {
		return 1;
	}
	else {
		return 0;
	}
}

int canWrite(char* mode) {
	if((strcmp(mode, "w") == 0) || (strcmp(mode, "w+") == 0) ||(strcmp(mode, "r+") == 0) 
		|| (strcmp(mode, "a") == 0)	|| (strcmp(mode, "a+") == 0)) {
		return 1;
	}
	else {
		return 0;
	}
}