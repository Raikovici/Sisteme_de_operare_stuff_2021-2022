#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include "configText.h"
int main(int argc, char* argv[])
{
	text T;
	T.nrLines = 0;
	T.capacityLines = MAX_SIZE;


	hashMap H;
	H.actualSize = 0;
	H.capacity = MAX_SIZE;

	char* filenameOut = (char*)malloc(sizeof(char) * 256);
	if (filenameOut == NULL) {
		printf("Can't allocate memory\n");
		exit(12);
	}
	char* filenameIn = (char*)malloc(sizeof(char) * 256);
	if (filenameIn == NULL) {
		printf("Can't allocate memory\n");
		exit(12);
	}

	char** vectorPaths = (char**)malloc(sizeof(char*) * 10);
	if (vectorPaths == NULL) {
		printf("Can't allocate memory\n");
		exit(12);
	}
	for (int i = 0; i < 10; i++) {
		vectorPaths[i] = (char*)malloc(sizeof(char) * 256);
		if (vectorPaths[i] == NULL) {
			printf("Can't allocate memory\n");
			exit(12);
		}
	}
	int nrPaths = 0;
	strcpy(filenameIn, "");
	strcpy(filenameOut, "");
	int skip = 0;
	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-D") == 0) {
			makeSymbol(&H, argv[i + 1]);
		}
		else if (argv[i][0] == '-' && argv[i][1] == 'D' && strlen(argv[i]) != 2) {
			char* s = (char*)malloc(sizeof(char) * (strlen(argv[i]) + 1));
			if (s == NULL) {
				printf("Can't allocate memory\n");
				exit(12);
			}
			char* ss = (char*)malloc(sizeof(char) * (strlen(argv[i]) + 1));
			if (ss == NULL) {
				printf("Can't allocate memory\n");
				exit(12);
			}
			strcpy(s, argv[i]);
			strcpy(ss, "");
			int p = 0;
			for (int i = 2; i < strlen(s); i++) {
				ss[p] = s[i];
				p++;
			}
			ss[p] = '\0';
			makeSymbol(&H, ss);
			if (s != NULL) {
				free(s);
				s = NULL;
			}
			if (ss != NULL) {
				free(ss);
				ss = NULL;
			}
		}
		else if (strcmp(argv[i], "-I") == 0) {
			char* path = (char*)malloc(sizeof(char) * (strlen(argv[i + 1]) + 1));
			if (path == NULL) {
				printf("Can't allocate memory\n");
				exit(12);
			}
			strcpy(path, argv[i + 1]);
			strcpy(vectorPaths[nrPaths], path);
			nrPaths++;
			if (path != NULL) {
				free(path);
				path = NULL;
			}
		}
		else if (argv[i][0] == '-' && argv[i][1] == 'I' && strlen(argv[i]) != 2) {
			char* s = (char*)malloc(sizeof(char) * (strlen(argv[i]) + 1));
			if (s == NULL) {
				printf("Can't allocate memory\n");
				exit(12);
			}
			strcpy(s, argv[i]);
			s += 2;
			char* path = (char*)malloc(sizeof(char) * (strlen(s) + 1));
			if (path == NULL) {
				printf("Can't allocate memory\n");
				exit(12);
			}
			strcpy(path, s);
			strcpy(vectorPaths[nrPaths], path);
			nrPaths++;
			if (path != NULL) {
				free(path);
				path = NULL;
			}
			if (s != NULL) {
				free(s);
				s = NULL;
			}
		}
		else if (strcmp(argv[i], "-o") == 0) {
			strcpy(filenameOut, argv[i + 1]);
		}
		else if (argv[i][0] == '-' && argv[i][1] == 'o' && strlen(argv[i]) != 2) {
			char* s = (char*)malloc(sizeof(char) * (strlen(argv[i]) + 1));
			if (s == NULL) {
				printf("Can't allocate memory\n");
				exit(12);
			}
			strcpy(s, argv[i]);
			s += 2;
			strcpy(filenameOut, s);
			if (s != NULL) {
				free(s);
				s = NULL;
			}
		}
		else if ((strcmp(argv[i - 1], "-o") == 0) || (strcmp(argv[i - 1], "-D") == 0) || (strcmp(argv[i - 1], "-I") == 0)) {
			skip = 1;
		}
		else {
			strcpy(filenameIn, argv[i]);
		}
	}
	makeLinesFromText(&T, filenameIn);
	putFlagsLines(&T);
	solveInclude(&T, vectorPaths, nrPaths);
	solveTextFile(&T, &H, filenameOut);
	free(T.filename);
	T.filename = NULL;
	eliberateText(&T);
	Destroy(&H);
	if (filenameOut != NULL) {
		free(filenameOut);
		filenameOut = NULL;
	}
	if (filenameIn != NULL) {
		free(filenameIn);
		filenameIn = NULL;
	}
	for (int i = 0; i < 10; i++) {
		if (vectorPaths[i] != NULL) {
			free(vectorPaths[i]);
			vectorPaths[i] = NULL;
		}
	}
	if (vectorPaths != NULL) {
		free(vectorPaths);
		vectorPaths = NULL;
	}
	return 0;
}
