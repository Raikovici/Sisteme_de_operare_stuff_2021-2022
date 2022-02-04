#pragma once

#include "hashmap.h"



typedef struct cond

{

	int trueOrFalse;

	char* expr;

	int flagIF;

	int flagELSE;

	int flagELIF;

	int flagENDIF;

	int flagIFDEF;

	int flagIFNDEF;

}cond;

void setType(cond* C, char _condition[256]);



void evaluateCondition(cond* C, hashMap* HM);



void destroyCond(cond* C);



int checkIF(cond* C, char* _cond);

int checkELIF(cond* C, char* _cond);

int checkELSE(cond* C, char* _cond);

int checkENDIF(cond* C, char* _cond);

int checkIFDEF(cond* C, char* _cond);

int checkIFNDEF(cond* C, char* _cond);