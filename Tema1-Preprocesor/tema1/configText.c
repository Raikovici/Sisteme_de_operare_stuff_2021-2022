#define _CRT_SECURE_NO_WARNINGS
#include "configText.h"
static int posBeginMulDefine = -1;
static int posEndMulDefine = -1;
static int posBeginIf = -1;
static int posEndIf = -1;
static int posElse = -1;
static int posElif = -1;
static int posIfdef = -1;
static int posIfndef = -1;
static int flagAnotherPath = -1;
static const char* PATH = "";
line* initLine(char* buffer)
{
	line* L = (line*)malloc(sizeof(line));
	if (L == NULL) {
		printf("Can't allocate memory\n");
		exit(12);
	}
	L->content = (char*)malloc(sizeof(char) * (strlen(buffer) + 2));
	setFlags(L);
	if (L->content == NULL) {
		printf("Can't allocate memory\n");
		exit(12);
	}
	strcpy(L->content, buffer);
	return L;
}

void makeLinesFromText(text* T, char* filename)
{
	T->filename = (char*)malloc(sizeof(char) * 256);
	if (T->filename == NULL) {
		printf("Can't allocate memory\n");
		exit(12);
	}
	strcpy(T->filename, filename);
	if (strcmp(T->filename, "") == 0) {
		int j = 0;
		char* buffer = (char*)malloc(sizeof(char) * 256);
		if (buffer == NULL) {
			printf("Can't allocate memory\n");
			exit(12);
		}
		fseek(stdin, 0, SEEK_SET);
		while (fgets(buffer, 256, stdin) != NULL) {
			line* L = initLine(buffer);
			T->vectLines[j] = L;
			T->nrLines++;
			j++;
			strcpy(buffer, "");
		}
		if (buffer != NULL) {
			free(buffer);
			buffer = NULL;
		}
	}
	else {
		int depth = 0;
		for (int i = 0; i < strlen(filename); i++) {
			if (filename[i] == '/') {
				depth++;
			}
		}
		if (depth >= 1) {
			flagAnotherPath = 1;
			strcpy(PATH, T->filename);
		}

		FILE* f;
		f = fopen(filename, "r+");
		if (f == NULL) {
			printf("Can't open the file\n");
			exit(5);
		}
		char* buffer = (char*)malloc(sizeof(char) * 256);
		if (buffer == NULL) {
			printf("Can't allocate memory\n");
			exit(12);
		}
		fseek(f, 0, SEEK_SET);
		int j = 0;
		while (!feof(f)) {
			fgets(buffer, 256, f);
			line* L = initLine(buffer);

			T->vectLines[j] = L;
			T->nrLines++;
			j++;
			strcpy(buffer, "");

		}
		if (buffer != NULL) {
			free(buffer);
			buffer = NULL;
		}
		fclose(f);
	}
	
}
void eliberateText(text* T)
{
	for (int i = 0; i < T->nrLines; i++) {
		if (T->vectLines[i]->content != NULL) {
			free(T->vectLines[i]->content);
			T->vectLines[i]->content = NULL;
		}
		if (T->vectLines[i] != NULL) {
			free(T->vectLines[i]);
			T->vectLines[i] = NULL;
		}
	}
}
void putFlagsLines(text* T)
{
	for (int i = 0; i < T->nrLines; i++) {
		cond C;
		C.expr = (char*)malloc(sizeof(char) * 256);
		if (C.expr == NULL) {
			printf("Can't allocate memory!\n");
			exit(12);
		}
		if (checkDefine(T->vectLines[i]->content) == 1) {
			T->vectLines[i]->flagSimpleDefine = 1;
			T->vectLines[i]->flagAppers = 0;
		}
		else if (checkIncludes(T->vectLines[i]->content) == 1) {
			T->vectLines[i]->flagInclude = 1;
			T->vectLines[i]->flagAppers = 0;
		}
		else if (checkMultipleDefine(T->vectLines[i]->content) == 1) {
			posBeginMulDefine = i;
			T->vectLines[i]->flagMultipleDefine = 1;
			T->vectLines[i]->flagAppers = 0;
		}
		else if (i > 0 && ((T->vectLines[i - 1]->flagMultipleDefine == 1) || (T->vectLines[i - 1]->flagInsideMultipleDefine == 1))) {
			if (checkInsideMultipleDefine(T->vectLines[i]->content) == 1) {
				T->vectLines[i]->flagInsideMultipleDefine = 1;
				T->vectLines[i]->flagAppers = 0;
			}
			else {
				if (checkEndMultipleDefine(T->vectLines[i]->content)) {
					T->vectLines[i]->flagEndMultipleDefine = 1;
					posEndMulDefine = i;
					T->vectLines[i]->flagAppers = 0;
				}
			}
		}
		else if (checkIF(&C, T->vectLines[i]->content) == 1) {
			T->vectLines[i + 1]->flagAfterIf = 1;
			posBeginIf = i;
			T->vectLines[i]->flagAppers = 0;
		}
		else if (checkELSE(&C, T->vectLines[i]->content) == 1) {
			T->vectLines[i + 1]->flagAfterElse = 1;
			posElse = i;
			T->vectLines[i]->flagAppers = 0;
		}
		else if (checkELIF(&C, T->vectLines[i]->content) == 1) {
			T->vectLines[i + 1]->flagAfterElif = 1;
			posElif = i;
			T->vectLines[i]->flagAppers = 0;
		}
		else if (checkIFDEF(&C, T->vectLines[i]->content) == 1) {
			T->vectLines[i + 1]->flagAfterIfdef = 1;
			if (checkDefine(T->vectLines[i + 1]->content)) {
				T->vectLines[i + 1]->flagSimpleDefine = 1;
			}
			posIfdef = i;
			T->vectLines[i]->flagAppers = 0;
		}
		else if (checkIFNDEF(&C, T->vectLines[i]->content) == 1) {
			T->vectLines[i + 1]->flagAfterIfndef = 1;
			if (checkDefine(T->vectLines[i + 1]->content)) {
				T->vectLines[i + 1]->flagSimpleDefine = 1;
			}
			posIfndef = i;
			T->vectLines[i]->flagAppers = 0;
		}
		else if (checkENDIF(&C, T->vectLines[i]->content) == 1) {
			T->vectLines[i + 1]->flagAfterEndif = 1;
			posEndIf = i;
			T->vectLines[i]->flagAppers = 0;
		}
		else if (checkUndef(T->vectLines[i]->content) == 1) {
			T->vectLines[i]->flagUndef = 1;
			T->vectLines[i]->flagAppers = 1;
		}
		else {
			T->vectLines[i]->flagAppers = 1;
		}
		destroyCond(&C);
	}
}
void interpretSimpleDefine(text* T, hashMap* H)
{
	for (int i = 0; i < T->nrLines; i++) {
		if (T->vectLines[i]->flagSimpleDefine == 1) {
			char* vectorTokens[2];
			int j = 0;
			char* stringg = (char*)malloc(sizeof(char) * (strlen(T->vectLines[i]->content) + 2));
			if (stringg == NULL) {
				printf("Can't allocate memory\n");
				exit(12);
			}
			strcpy(stringg, T->vectLines[i]->content);
			stringg[strlen(stringg) - 1] = '\0';
			char* ptr = stringg;
			vectorTokens[j++] = stringg;
			while (*ptr)
			{
				if (*ptr == ' ') {
					*ptr = 0;
					vectorTokens[j++] = ptr + 1;
					if (j == 2) {
						break;
					}
				}
				ptr++;
			}
			makeSymbol(H, vectorTokens[1]);
			if (stringg != NULL) {
				free(stringg);
				stringg = NULL;
			}
		}
	}
}
void interpretMulDefine(text* T, hashMap* H)
{
	char* buffer = (char*)malloc(sizeof(char) * 256);
	if (buffer == NULL) {
		printf("Can't allocate memory\n");
		exit(12);
	}
	char* symbol = (char*)malloc(sizeof(char) * 256);
	if (symbol == NULL) {
		printf("Can't allocate memory!\n");
		exit(12);
	}
	char* finalValue = (char*)malloc(sizeof(char) * 256);
	if (finalValue == NULL) {
		printf("Can't allocate memory!\n");
		exit(12);
	}
	char* buffer2 = (char*)malloc(sizeof(char) * 256);
	if (buffer2 == NULL) {
		printf("Can't allocate memory\n");
		exit(12);
	}
	char* key_and_val = (char*)malloc(sizeof(char) * 256);
	if (key_and_val == NULL) {
		printf("Can't allocate memory\n");
		exit(12);
	}
	if (posBeginMulDefine != -1 && posEndMulDefine != -1) {
		strcpy(buffer, T->vectLines[posBeginMulDefine]->content);
		buffer[strlen(buffer) - 1] = '\0';
		char* token = strtok(buffer, " ");
		int cnt = 0;
		while (token != NULL) {
			if (cnt == 1) {
				strcpy(symbol, token);
			}
			else if (cnt == 2) {
				strcpy(finalValue, token);
			}
			token = strtok(NULL, " ");
			cnt++;
		}
		for (int k = posBeginMulDefine + 1; k <= posEndMulDefine; k++)
		{
			strcpy(buffer2, T->vectLines[k]->content);
			if (buffer2[strlen(buffer2) - 2] == '\\') {
				buffer2[strlen(buffer2) - 1] = '\0';
				buffer2[strlen(buffer2) - 1] = '\0';
			}
			if (buffer2[strlen(buffer2) - 1] == '\n' && buffer2[strlen(buffer2) - 2] != '\\') {
				buffer2[strlen(buffer2) - 1] = '\0';
			}
			token = strtok(buffer2, " ");
			int t = 0;
			while (token != NULL) {
				if (t == 0) {
					strcat(finalValue, " ");
					strcat(finalValue, token);
				}
				if (t == 1) {
					strcat(finalValue, " ");
					strcat(finalValue, token);
				}
				token = strtok(NULL, " ");
				t++;
			}
		}
		strcpy(key_and_val, symbol);
		strcat(key_and_val, " ");
		strcat(key_and_val, finalValue);
		makeSymbol(H, key_and_val);
	}
	posBeginMulDefine = -1;
	posEndMulDefine = -1;
	if (buffer != NULL) {
		free(buffer);
		buffer = NULL;
	}
	if (symbol != NULL) {
		free(symbol);
		symbol = NULL;
	}
	if (finalValue != NULL) {
		free(finalValue);
		finalValue = NULL;
	}
	if (buffer2 != NULL) {
		free(buffer2);
		buffer2 = NULL;
	}
	if (key_and_val != NULL) {
		free(key_and_val);
		key_and_val = NULL;
	}
}
void interpretIf_EndifBlock(text* T, hashMap* H)
{
	cond C_IF;
	cond C_ELSE;
	cond C_ELIF;
	C_IF.expr = (char*)malloc(sizeof(char) * 256);
	if (C_IF.expr == NULL) {
		printf("Can't allocate memory!\n");
		exit(12);
	}
	C_ELSE.expr = (char*)malloc(sizeof(char) * 256);
	if (C_ELSE.expr == NULL) {
		printf("Can't allocate memory!\n");
		exit(12);
	}
	C_ELIF.expr = (char*)malloc(sizeof(char) * 256);
	if (C_ELIF.expr == NULL) {
		printf("Can't allocate memory!\n");
		exit(12);
	}
	if (posBeginIf != -1 && posEndIf != -1) {
		setType(&C_IF, T->vectLines[posBeginIf]->content);
		evaluateCondition(&C_IF, H);
		if (T->vectLines[posBeginIf + 1]->flagAfterIf == 1) {
			if (C_IF.trueOrFalse == 1) {
				T->vectLines[posBeginIf + 1]->flagAppers = 1;
			}
			else {
				T->vectLines[posBeginIf + 1]->flagAppers = 0;
			}
		}
		if (posElif != -1) {
			setType(&C_ELIF, T->vectLines[posElif]->content);
			evaluateCondition(&C_ELIF, H);
			if (T->vectLines[posElif + 1]->flagAfterElif == 1) {
				if (C_ELIF.trueOrFalse == 1) {
					T->vectLines[posElif + 1]->flagAppers = 1;
				}
				else {
					T->vectLines[posElif + 1]->flagAppers = 0;
				}
			}
		}
		if (posElse != -1) {
			setType(&C_ELSE, T->vectLines[posElse]->content);
			if (T->vectLines[posElse + 1]->flagAfterElse == 1) {
				if (C_IF.trueOrFalse == 0) {
					if (posElif != -1) {
						if (C_ELIF.trueOrFalse == 1) {
							T->vectLines[posElse + 1]->flagAppers = 0;
						}
					}
					else {
						T->vectLines[posElse + 1]->flagAppers = 1;
					}
				}
				else {
					T->vectLines[posElse + 1]->flagAppers = 0;
				}
			}
		}
		cond C_ENDIF;
		C_ENDIF.expr = (char*)malloc(sizeof(char) * 256);
		if (C_ENDIF.expr == NULL) {
			printf("Can't allocate memory!\n");
			exit(12);
		}
		setType(&C_ENDIF, T->vectLines[posEndIf]->content);
		if (T->vectLines[posEndIf + 1]->flagAfterEndif == 1) {
			T->vectLines[posEndIf + 1]->flagAppers = 1;
		}
		if (posEndIf != -1) {
			destroyCond(&C_ENDIF);
		}
	}

	destroyCond(&C_IF);

	destroyCond(&C_ELSE);

	destroyCond(&C_ELIF);

}
void interpretIfdef_IfndefBlock(text* T, hashMap* H)
{
	if ((posIfdef != -1 || posIfndef != -1) && posEndIf != -1) {
		if (posIfdef != -1) {
			cond C_IFDEF;
			C_IFDEF.expr = (char*)malloc(sizeof(char) * 256);
			if (C_IFDEF.expr == NULL) {
				printf("Can't allocate memory!\n");
				exit(12);
			}
			setType(&C_IFDEF, T->vectLines[posIfdef]->content);
			evaluateCondition(&C_IFDEF, H);
			if (T->vectLines[posIfdef + 1]->flagAfterIfdef == 1) {
				if (T->vectLines[posIfdef + 1]->flagSimpleDefine == 1) {
					T->vectLines[posIfdef + 1]->flagAppers = 0;
				}
				else
				{
					if (C_IFDEF.trueOrFalse == 1) {
						T->vectLines[posIfdef + 1]->flagAppers = 1;
					}
					else {
						T->vectLines[posIfdef + 1]->flagAppers = 0;
					}
				}
			}
			destroyCond(&C_IFDEF);
		}
		if (posIfndef != -1) {
			cond C_IFNDEF;
			C_IFNDEF.expr = (char*)malloc(sizeof(char) * 256);
			if (C_IFNDEF.expr == NULL) {
				printf("Can't allocate memory!\n");
				exit(12);
			}
			setType(&C_IFNDEF, T->vectLines[posIfndef]->content);
			evaluateCondition(&C_IFNDEF, H);
			if (T->vectLines[posIfndef + 1]->flagAfterIfndef == 1) {
				if (T->vectLines[posIfndef + 1]->flagSimpleDefine == 1) {
					T->vectLines[posIfndef + 1]->flagAppers = 0;
				}
				else
				{
					if (C_IFNDEF.trueOrFalse == 1) {
						T->vectLines[posIfndef + 1]->flagAppers = 0;
					}
					else {
						T->vectLines[posIfndef + 1]->flagAppers = 1;
					}
				}
			}
			destroyCond(&C_IFNDEF);
			cond C_ENDIF;
			C_ENDIF.expr = (char*)malloc(sizeof(char) * 256);
			if (C_ENDIF.expr == NULL) {
				printf("Can't allocate memory!\n");
				exit(12);
			}
			setType(&C_ENDIF, T->vectLines[posEndIf]->content);
			if (T->vectLines[posEndIf + 1]->flagAfterEndif == 1) {
				T->vectLines[posEndIf + 1]->flagAppers = 1;
			}
			destroyCond(&C_ENDIF);
		}
	}
}
void interpretSymbols(hashMap* H, char* _string, char* _newLine)
{
	char* buffer = (char*)malloc(sizeof(char) * (strlen(_string) + 2));
	if (buffer == NULL) {
		printf("Can't allocate memory\n");
		exit(12);
	}
	strcpy(buffer, _string);
	char* delims = (char*)malloc(sizeof(char) * 256);
	if (delims == NULL) {
		printf("Can't allocate memory\n");
		exit(12);
	}
	strcpy(delims, "\t []{}<>=+-*/%!&|^.,:;()");
	char* token = strtok(buffer, delims);
	char* newLinee = (char*)malloc(sizeof(char) * (strlen(_string) + 2));
	if (newLinee == NULL) {
		printf("Can't allocate memory\n");
		exit(12);
	}
	char* oldLine = (char*)malloc(sizeof(char) * (strlen(_string) + 2));
	if (oldLine == NULL) {
		printf("Can't allocate memory\n");
		exit(12);
	}
	strcpy(oldLine, _string);
	char* val = (char*)malloc(sizeof(char) * 256);
	if (val == NULL) {
		printf("Can't allocate memory\n");
		exit(12);
	}
	char* oldToken = (char*)malloc(sizeof(char) * 256);
	if (oldToken == NULL) {
		printf("Can't allocate memory\n");
		exit(12);
	}
	int OK = 0;
	int nrTokens = 0;
	while (token != NULL) {
		if (found(H, token) == 1) {
			getValue(H, token, val);
			strcpy(oldToken, token);
			OK = 1;
			nrTokens++;
			if (OK == 1) {
				if (nrTokens >= 2) {
					strcpy(oldLine, _newLine);
				}
				oldLine[strlen(oldLine) - 1] = '\0';
				int i = 0, j = 0, flag = 0, start = 0;
				while (oldLine[i] != '\0') {
					if (oldLine[i] == oldToken[j] && oldLine[i - 1] != '\"') {
						if (!flag) {
							start = i;
						}
						j++;
						if (oldToken[j] == '\0') {
							flag = 1;
							break;
						}
						flag = 1;
					}
					else {
						flag = start = j = 0;
					}
					i++;
				}
				if (oldToken[j] == '\0' && flag) {
					for (i = 0; i < start; i++) {
						_newLine[i] = oldLine[i];
					}
					for (j = 0; j < strlen(val); j++) {
						_newLine[i] = val[j];
						i++;
					}
					for (j = start + strlen(oldToken); j < strlen(oldLine); j++) {
						_newLine[i] = oldLine[j];
						i++;
					}
					_newLine[i] = '\0';
				}
			}
		}
		
		
		token = strtok(NULL, delims);
	}
	
	if (OK == 0) {
		strcpy(_newLine, _string);
	}

	if (buffer != NULL) {
		free(buffer);
		buffer = NULL;
	}
	if (oldLine != NULL) {
		free(oldLine);
		oldLine = NULL;
	}
	if (newLinee != NULL) {
		free(newLinee);
		newLinee = NULL;
	}
	if (oldToken != NULL) {
		free(oldToken);
		oldToken = NULL;
	}
	if (val != NULL) {
		free(val);
		val = NULL;
	}
	if (delims != NULL) {
		free(delims);
		delims = NULL;
	}

}
void solveInclude(text* T, char** vectPaths, int nrPaths)
{
	text subText;
	subText.nrLines = 0;
	subText.capacityLines = MAX_SIZE;

	for (int i = 0; i < T->nrLines; i++) {
		if (T->vectLines[i]->flagInclude == 1) {
			char* filename = getIncludeFile(T->vectLines[i]->content);
			if (validPath(filename, vectPaths, nrPaths) == 0) {
				printf("Can't find the correct path!");
				exit(2);
			}
			makeLinesFromText(&subText, filename);
			putFlagsLines(&subText);
			if (foundIncludeDirective(&subText) == 1) {
				solveInclude(&subText, vectPaths, nrPaths);
			}
			T = addSubtextToText(T, &subText, i);
		}
	}
	eliberateText(&subText);
}
int validPath(char* filename, char** vectPaths, int nrPaths)
{
	char* completePath = (char*)malloc(sizeof(char) * 256);
	if (completePath == NULL) {
		printf("Can't allocate memory");
		exit(12);
	}
	int flagPaths = 0;
	for (int i = 0; i < nrPaths; i++) {
		strcpy(completePath, vectPaths[i]);
		strcat(completePath, "/");
		strcat(completePath, filename);
		completePath[strlen(completePath)] = '\0';
		FILE* f = fopen(completePath, "r");
		if (f != NULL) {
			flagPaths = 1;
			fclose(f);
		}
		strcpy(completePath, "");
	}
	if (completePath != NULL) {
		free(completePath);
		completePath = NULL;
	}

	int flagDirCurent = 0;
	FILE* f = fopen(filename, "r");
	if (f != NULL) {
		flagDirCurent = 1;
		fclose(f);
	}
	int flagAnotherDir = 0;
	if (flagAnotherPath == 1) {
		int pos = 0;
		for (int j = strlen(PATH); j > 0; j--) {
			if (PATH[j] == '/') {
				pos = j;
				break;
			}
		}
		int posGood = strlen(PATH) - pos;
		char* semiPath = (char*)malloc(sizeof(char) * (strlen(PATH) + 1));
		if (semiPath == NULL) {
			printf("Can't allocate memory\n");
			exit(12);
		}
		for (int i = 0; i < posGood; i++) {
			semiPath[i] = PATH[i];
		}
		strcat(semiPath, '/');
		strcat(semiPath, filename);
		FILE* f = fopen(semiPath, "r");
		if (f != NULL) {
			flagAnotherDir = 1;
			fclose(f);
		}
		if (semiPath != NULL) {
			free(semiPath);
			semiPath = NULL;
		}

	}
	return flagAnotherDir || flagDirCurent || flagPaths;
}
int foundIncludeDirective(text* T)
{
	int ok = 0;
	for (int i = 0; i < T->nrLines; i++) {
		if (T->vectLines[i]->flagInclude == 1) {
			ok = 1;
		}
	}
	return ok;
}
text* addSubtextToText(text* T, text* textt, int pos)
{
	text t1;
	t1.capacityLines = MAX_SIZE;
	t1.nrLines = 0;
	for (int i = 0; i < t1.capacityLines; i++) {
		t1.vectLines[i] = (line*)malloc(sizeof(line));
	}
	int k = 0;
	for (int i = pos + 1; i < T->nrLines; i++) {
		line* L = initLine(T->vectLines[i]->content);
		t1.vectLines[k] = L;
		t1.nrLines++;
		k++;
	}
	T->nrLines += textt->nrLines;
	T->nrLines -= 1;
	for (int i = 0; i < textt->nrLines; i++) {
		line* L = initLine(textt->vectLines[i]->content);

		T->vectLines[pos + i] = L;
	}
	for (int i = 0; i < t1.nrLines; i++) {
		line* L = initLine(t1.vectLines[i]->content);

		T->vectLines[pos + i + textt->nrLines] = L;
	}
	eliberateText(&t1);
	return T;
}
void undefSymbol(char* _string, hashMap* H) {
	char* buffer = (char*)malloc(sizeof(char) * (strlen(_string) + 2));
	if (buffer == NULL) {
		printf("Can't allocate memory\n");
		exit(12);
	}
	strcpy(buffer, _string);
	buffer[strlen(buffer) - 1] = '\0';
	char* token = strtok(buffer, " ");
	if (token != NULL) {
		token = strtok(NULL, " ");
		if (token != NULL) {
			for (int i = 0; i < H->actualSize; i++) {
				if (strcmp(H->items[i]->_key, token) == 0) {
					H->items[i]->isDefined = 0;
					strcpy(H->items[i]->_value, "");
				}
			}
		}
	}
	if (buffer != NULL) {
		free(buffer);
		buffer = NULL;
	}
}
void solveTextFile(text* T, hashMap* H, char* outputFilename)
{
	resetPositions();
	putFlagsLines(T);
	interpretSimpleDefine(T, H);
	interpretIf_EndifBlock(T, H);
	interpretMulDefine(T, H);
	interpretIfdef_IfndefBlock(T, H);
	FILE* out;
	if (strcmp(outputFilename, "") == 0) {
		out = stdout;
	}
	else {
		out = fopen(outputFilename, "w");
	}
	for (int i = 0; i < T->nrLines; i++)
	{
		if (T->vectLines[i]->flagAppers == 1) {
			if (checkUndef(T->vectLines[i]->content) == 1) {
				undefSymbol(T->vectLines[i]->content, H);
				T->vectLines[i]->flagAppers = 0;
			}
			if (T->vectLines[i]->flagUndef != 1) {
				char* newLine = (char*)malloc(sizeof(char) * 256);
				if (newLine == NULL) {
					printf("Can't allocate memory\n");
					exit(12);
				}

				interpretSymbols(H, T->vectLines[i]->content, newLine);
				newLine[strlen(newLine)] = '\0';
				if ((newLine[strlen(newLine) - 1] == '\n') && (strcmp(newLine, "\n") != 0)) {
					//printf("%s", newLine);
					fprintf(out, "%s", newLine);
				}
				else if (strcmp(newLine, "\n") == 0) {
					//printf("\n");
					fprintf(out, "%s", "\n");
				}
				else {
					//printf("%s\n", newLine);
					fprintf(out, "%s\n", newLine);
				}

				free(newLine);
				newLine = NULL;

			}
		}
	}
	fclose(out);
}
void resetPositions()
{
	posBeginMulDefine = -1;
	posEndMulDefine = -1;
	posBeginIf = -1;
	posEndIf = -1;
	posElse = -1;
	posElif = -1;
	posIfdef = -1;
	posIfndef = -1;
}
void setFlags(line* L)
{
	L->flagAfterElif = -1;
	L->flagAfterElse = -1;
	L->flagAfterEndif = -1;
	L->flagAfterIf = -1;
	L->flagAfterIfdef = -1;
	L->flagAfterIfndef = -1;
	L->flagEndMultipleDefine = -1;
	L->flagInclude = -1;
	L->flagInsideMultipleDefine = -1;
	L->flagMultipleDefine = -1;
	L->flagNormal = -1;
	L->flagSimpleDefine = -1;
	L->flagAppers = -1;
}
