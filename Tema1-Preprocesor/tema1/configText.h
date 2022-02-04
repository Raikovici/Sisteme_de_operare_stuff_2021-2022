#pragma once
#include "hashmap.h"
#include "defines.h"
#include "includes.h"
#include "conditions.h"


typedef struct line 
{
	char* content;
	int flagNormal;
	int flagSimpleDefine;
	int flagMultipleDefine;
	int flagInsideMultipleDefine;
	int flagEndMultipleDefine;
	int flagAfterIf;
	int flagAfterElse;
	int flagAfterElif;
	int flagAfterIfdef;
	int flagAfterIfndef;
	int flagAfterEndif;
	int flagInclude;
	int flagUndef;

	int flagAppers;

}line;

typedef struct text
{
	line* vectLines[MAX_SIZE];
	int nrLines;
	char* filename;
	int capacityLines;
}text;

line* initLine(char* buffer);



void makeLinesFromText(text* T, char* filename);

void eliberateText(text* T);

void putFlagsLines(text* T);

void interpretMulDefine(text* T, hashMap* H);

void interpretSimpleDefine(text* T, hashMap* H);

void resetPositions();

void setFlags(line* L);

void interpretIf_EndifBlock(text* T, hashMap* H);

void solveTextFile(text* T, hashMap* H, char* outputFilename);

void interpretIfdef_IfndefBlock(text* T, hashMap* H);

void undefSymbol(char* _string, hashMap* H);

void interpretSymbols(hashMap* H, char* _string, char* _newLine);

void solveInclude(text* T, char** vectPaths, int nrPaths);

int foundIncludeDirective(text* T);

int validPath(char* filename, char** vectPaths, int nrPaths);

text* addSubtextToText(text* T, text* textt, int pos);