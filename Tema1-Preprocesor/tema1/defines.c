#define _CRT_SECURE_NO_WARNINGS
#include "hashmap.h"
#include "defines.h"
void makeSymbol(hashMap* HM, char* _string)
{
	char* _key = (char*)malloc(sizeof(char) * MAX_SIZE);
	if (_key == NULL) {
		printf("Can't allocate memory\n");
		exit(12);
	}
	char* _value = (char*)malloc(sizeof(char) * MAX_SIZE);
	if (_value == NULL) {
		printf("Can't allocate memory\n");
		exit(12);
	}
	char* stringg = (char*)malloc(sizeof(char) * (strlen(_string) + 2));
	if (stringg == NULL) {
		printf("Can't allocate memory\n");
		exit(12);
	}
	char* stringg2 = (char*)malloc(sizeof(char) * (strlen(_string) + 2));
	if (stringg2 == NULL) {
		printf("Can't allocate memory\n");
		exit(12);
	}
	int size = 0;
	strcpy(stringg, _string);
	strcpy(stringg2, _string);
	char* vectorTokens[2];
	int i = 0;
	char* ptr = stringg;
	vectorTokens[i++] = stringg;
	while (*ptr)
	{
		if (*ptr == ' ' || *ptr == '=') {
			*ptr = 0;
			vectorTokens[i++] = ptr + 1;
			if (i == 2) {
				break;
			}
		}
		ptr++;
	}
	if (i == 1) {
		size = 1;
	}
	else {
		size = 2;
	}
	if (size == 1) {
		strcpy(_key, vectorTokens[0]);
		strcpy(_value, "");
	}
	else if (size == 2) {
		strcpy(_key, vectorTokens[0]);
		strcpy(_value, vectorTokens[1]);
	}
	dataItem* newDI = initDataItem(_key, _value);
	int gasit = 0;
	for (int i = 0; i < HM->actualSize; i++) {
		if (strcmp(HM->items[i]->_key, _key) == 0) {
			strcpy(HM->items[i]->_value, _value);
			gasit = 1;
		}
	}
	if (gasit == 0) {
		HM->items[HM->actualSize] = newDI;
		HM->actualSize++;
	}
	if (stringg != NULL) {
		free(stringg);
		stringg = NULL;
	}
	if (stringg2 != NULL) {
		free(stringg2);
		stringg2 = NULL;
	}
	if (_key != NULL) {
		free(_key);
		_key = NULL;
	}
	if (_value != NULL) {
		free(_value);
		_value = NULL;
	}
}
int checkDefine(char* _string)
{
	char* token;
	char* stringg = (char*)malloc(sizeof(char) * (strlen(_string) + 2));
	if (stringg == NULL) {
		printf("Can't allocate memory\n");
		exit(12);
	}
	char* stringg2 = (char*)malloc(sizeof(char) * (strlen(_string) + 2));
	if (stringg2 == NULL) {
		printf("Can't allocate memory\n");
		exit(12);
	}
	strcpy(stringg, _string);
	strcpy(stringg2, _string);
	stringg[strlen(stringg)] = '\0';
	stringg2[strlen(stringg2)] = '\0';
	int flagSimple = 0, flagDefine = 0;
	token = strtok(stringg, " ");
	if (token != NULL) {
		if (strcmp(token, "#define") == 0) {
			flagDefine = 1;
		}
		else {
			flagDefine = 0;
		}
		if (stringg2[strlen(stringg2) - 2] != '\\') {
			flagSimple = 1;
		}
	}
	if (stringg != NULL) {
		free(stringg);
		stringg = NULL;
	}
	if (stringg2 != NULL) {
		free(stringg2);
		stringg2 = NULL;
	}
	return flagDefine && flagSimple;
}
int checkMultipleDefine(char* _string)
{
	char* token;
	char* stringg = (char*)malloc(sizeof(char) * (strlen(_string) + 2));
	if (stringg == NULL) {
		printf("Can't allocate memory\n");
		exit(12);
	}
	char* stringg2 = (char*)malloc(sizeof(char) * (strlen(_string) + 2));
	if (stringg2 == NULL) {
		printf("Can't allocate memory\n");
		exit(12);
	}
	strcpy(stringg, _string);
	strcpy(stringg2, _string);
	stringg[strlen(stringg)] = '\0';
	stringg2[strlen(stringg2)] = '\0';
	int flagMultiple = 0, flagDefine = 0;
	token = strtok(stringg, " ");
	if (token != NULL) {
		if (strcmp(token, "#define") == 0) {
			flagDefine = 1;
		}
		else {
			flagDefine = 0;
		}
		if (stringg2[strlen(stringg2) - 2] == '\\' && stringg2[strlen(stringg2) - 1] == '\n')
		{
			flagMultiple = 1;
		}
	}
	if (stringg != NULL) {
		free(stringg);
		stringg = NULL;
	}
	if (stringg2 != NULL) {
		free(stringg2);
		stringg2 = NULL;
	}
	return flagDefine && flagMultiple;
}
int checkInsideMultipleDefine(char* _string)
{
	char* stringg = (char*)malloc(sizeof(char) * (strlen(_string) + 2));
	if (stringg == NULL) {
		printf("Can't allocate memory\n");
		exit(12);
	}
	strcpy(stringg, _string);
	int flag1 = 0, flag2 = 0;
	if (stringg[0] == ' ')
	{
		flag1 = 1;
	}
	if (stringg[strlen(stringg) - 2] == '\\') {
		flag2 = 1;
	}
	if (stringg != NULL) {
		free(stringg);
		stringg = NULL;
	}
	return flag1 && flag2;
}
int checkEndMultipleDefine(char* _string)
{
	char* stringg = (char*)malloc(sizeof(char) * (strlen(_string) + 2));
	if (stringg == NULL) {
		printf("Can't allocate memory\n");
		exit(12);
	}
	strcpy(stringg, _string);
	int flag1 = 0, flag2 = 0, flag3 = 0;
	if (stringg[0] == ' ') {
		flag1 = 1;
	}
	if (stringg[strlen(stringg) - 2] != '\\') {
		flag2 = 1;
	}
	if (stringg[strlen(stringg) - 1] == '\n') {
		flag3 = 1;
	}
	if (stringg != NULL) {
		free(stringg);
		stringg = NULL;
	}
	return flag1 && flag2 && flag3;
}
int checkUndef(char* _string)
{
	char* token;
	char* stringg = (char*)malloc(sizeof(char) * (strlen(_string) + 2));
	if (stringg == NULL) {
		printf("Can't allocate memory\n");
		exit(12);
	}
	strcpy(stringg, _string);
	int flagUndef = 0;
	token = strtok(stringg, " ");
	if (token != NULL) {
		if (strcmp(token, "#undef") == 0) {
			flagUndef = 1;
		}
		else {
			flagUndef = 0;
		}
	}
	if (stringg != NULL) {
		free(stringg);
		stringg = NULL;
	}
	return flagUndef;
}
