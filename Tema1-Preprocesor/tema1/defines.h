#pragma once
#include "hashmap.h"

void makeSymbol(hashMap* HM, char* _string);
int checkDefine(char* _string);
int checkMultipleDefine(char* _string);
int checkInsideMultipleDefine(char* _string);
int checkEndMultipleDefine(char* _string);
int checkUndef(char* _string);