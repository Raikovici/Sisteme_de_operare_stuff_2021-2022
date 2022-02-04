#define _CRT_SECURE_NO_WARNINGS
#include "conditions.h"
void setType(cond* C, char _condition[256])
{
	char* delim = " \n";
	char* token;
	C->flagIF = -1;
	C->flagELSE = -1;
	C->flagELIF = -1;
	C->flagENDIF = -1;
	C->flagIFDEF = -1;
	C->flagIFNDEF = -1;
	char* stringg = (char*)malloc(sizeof(char) * MAX_SIZE);
	if (stringg == NULL) {
		printf("Can't allocate memory\n");
		exit(12);
	}
	strcpy(stringg, _condition);
	token = strtok(stringg, delim);
	int i = 0;
	while (token != NULL) {
		if (i == 0) {
			if (strcmp(token, "#if") == 0) {
				C->flagIF = 1;
			}
			else if ((strcmp(token, "#else") == 0)) {
				C->flagELSE = 1;
			}
			else if ((strcmp(token, "#elif") == 0)) {
				C->flagELIF = 1;
			}
			else if (strcmp(token, "#endif") == 0) {
				C->flagENDIF = 1;
			}
			else if (strcmp(token, "#ifdef") == 0) {
				C->flagIFDEF = 1;
			}
			else if (strcmp(token, "#ifndef") == 0) {
				C->flagIFNDEF = 1;
			}
		}
		if (i == 1) {
			strcpy(C->expr, token);
		}
		token = strtok(NULL, delim);
		i++;
	}
	if (stringg != NULL) {
		free(stringg);
		stringg = NULL;
	}
}
void evaluateCondition(cond* C, hashMap* HM)
{
	char* value = (char*)malloc(sizeof(char) * 256);
	if (value == NULL) {
		exit(12);
	}
	if (C->flagIF == 1) {
		if (strcmp(C->expr, "1") == 0) {
			C->trueOrFalse = 1;
		}
		else if (strcmp(C->expr, "0") == 0) {
			C->trueOrFalse = 0;
		}
		else {
			if (HM->actualSize != 0) {
				getValue(HM, C->expr, value);
				if (strcmp(value, "1") == 0) {
					C->trueOrFalse = 1;
				}
				else if (strcmp(value, "0") == 0) {
					C->trueOrFalse = 0;
				}
			}
		}
	}
	else if (C->flagELIF == 1)
	{
		if (strcmp(C->expr, "1") == 0) {
			C->trueOrFalse = 1;
		}
		else if (strcmp(C->expr, "0") == 0) {
			C->trueOrFalse = 0;
		}
		else {
			if (HM->actualSize != 0) {
				getValue(HM, C->expr, value);
				if (strcmp(value, "1") == 0) {
					C->trueOrFalse = 1;
				}
				else if (strcmp(value, "0") == 0) {
					C->trueOrFalse = 0;
				}
			}
		}
	}
	else if (C->flagIFDEF == 1) {
		int gasit = found(HM, C->expr);
		if (gasit == 1) {
			C->trueOrFalse = 1;
		}
		else if (gasit == 0) {
			C->trueOrFalse = 0;
		}
	}
	else if (C->flagIFNDEF == 1) {
		int gasit = found(HM, C->expr);
		if (gasit == 0) {
			C->trueOrFalse = 1;
		}
		else if (gasit == 1) {
			C->trueOrFalse = 0;
		}
	}
	if (value != NULL) {
		free(value);
		value = NULL;
	}
}
int checkIF(cond* C, char* _cond)
{
	setType(C, _cond);
	return C->flagIF == 1;
}
int checkELIF(cond* C, char* _cond)
{
	setType(C, _cond);
	return C->flagELIF == 1;
}
int checkELSE(cond* C, char* _cond)
{
	setType(C, _cond);
	return C->flagELSE == 1;
}
int checkENDIF(cond* C, char* _cond)
{
	setType(C, _cond);
	return C->flagENDIF == 1;
}
int checkIFDEF(cond* C, char* _cond)
{
	setType(C, _cond);
	return C->flagIFDEF == 1;
}
int checkIFNDEF(cond* C, char* _cond)
{
	setType(C, _cond);
	return C->flagIFNDEF == 1;
}
void destroyCond(cond* C)
{
	if (C->expr != NULL) {
		free(C->expr);
		C->expr = NULL;
	}
}
