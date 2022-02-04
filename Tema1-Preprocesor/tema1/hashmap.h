#pragma once
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define MAX_SIZE 256

typedef struct dataItem
{
	char* _key;
	char* _value;
	int isDefined;
}dataItem;

typedef struct hashMap
{
	dataItem* items[MAX_SIZE];
	int capacity;
	int actualSize;
}hashMap;

dataItem* initDataItem(char* _key, char* _value);
void getValue(hashMap* HM, char* _key, char* _value);
void Destroy(hashMap* HM);
void printHM(hashMap* HM);
int found(hashMap* HM, char* _key);