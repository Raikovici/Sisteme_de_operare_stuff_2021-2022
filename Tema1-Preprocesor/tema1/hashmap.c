#define _CRT_SECURE_NO_WARNINGS
#include "hashmap.h"
dataItem* initDataItem(char* _key, char* _value)
{
	dataItem* newDI = (dataItem*)malloc(sizeof(dataItem));
	if (newDI == NULL) {
		printf("Can't allocate memory\n");
		exit(12);
	}
	newDI->_key = (char*)malloc(sizeof(char) * (strlen(_key) + 1));
	if (newDI->_key == NULL) {
		printf("Can't allocate memory for key\n");
		exit(12);
	}
	strcpy(newDI->_key, _key);
	newDI->_value = (char*)malloc(sizeof(char) * (strlen(_value) + 1));
	if (newDI->_value == NULL) {
		printf("Can't allocate memory for value\n");
		exit(12);
	}
	strcpy(newDI->_value, _value);
	newDI->isDefined = 1;
	return newDI;
}
void Destroy(hashMap* HM)
{
	for (int i = 0; i < HM->actualSize; i++) {
		if (HM->items[i]->_key != NULL) {
			free(HM->items[i]->_key);
			HM->items[i]->_key = NULL;
		}
		if (HM->items[i]->_value != NULL) {
			free(HM->items[i]->_value);
			HM->items[i]->_value = NULL;
		}
		if (HM->items[i] != NULL) {
			free(HM->items[i]);
			HM->items[i] = NULL;
		}
	}

}
void getValue(hashMap* HM, char* _key, char* _value)
{
	int comp = 0;
	
	for (int i = 0; i < HM->actualSize; i++) {
		if (strcmp(HM->items[i]->_key, _key) == 0) {
			strcpy(_value, HM->items[i]->_value);
			
			for (int j = 0; j < HM->actualSize; j++) {
				if (strstr(_value, HM->items[j]->_key) != NULL) {
					comp = 1;
				}
			}
		}
	}
	if (comp == 1) {
		char* oldVal = (char*)malloc(sizeof(char) * (strlen(_value) + 1));
		if (oldVal == NULL) {
			printf("Can't allocate memory for value\n");
			exit(12);
		}
		int flag = 0;
		strcpy(oldVal, _value);
		char* token = strtok(oldVal, " ");
		while (token != NULL) {
			for (int i = 0; i < HM->actualSize; i++) {
				if (strcmp(HM->items[i]->_key, token) == 0) {
					strcpy(_value, HM->items[i]->_value);
					flag = 1;
				}
			}
			if (flag == 0) {
				strcat(_value, " ");
				strcat(_value, token);
			}
			
			
			flag = 0;
			token = strtok(NULL, " ");
		}
		if (oldVal != NULL) {
			free(oldVal);
			oldVal = NULL;
		}
	}
}
void printHM(hashMap* HM)
{
	for (int i = 0; i < HM->actualSize; i++)
	{
		printf("Cheia este %s si valoarea este %s\n", HM->items[i]->_key, HM->items[i]->_value);
	}
}

int found(hashMap* HM, char* _key)
{
	int flag = 0;
	for (int i = 0; i < HM->actualSize; i++) {
		if (strcmp(HM->items[i]->_key, _key) == 0) {
			flag = 1;
		}
	}
	return flag;
}

