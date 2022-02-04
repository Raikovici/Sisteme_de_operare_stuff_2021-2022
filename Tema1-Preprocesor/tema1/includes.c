#define _CRT_SECURE_NO_WARNINGS
#include "includes.h"

int checkIncludes(char* _string)
{
	char* stringg = (char*)malloc(sizeof(char) * (strlen(_string) + 1));
	if (stringg == NULL) {
		printf("Can't allocate memory\n");
		exit(12);
	}
	strcpy(stringg, _string);
	stringg[strlen(stringg) - 1] = '\0';
	char* token = strtok(stringg, " ");
	int flagInclude = 0, flagOkFilename = 0, i = 0 ;
	while (token != NULL) {
		if (i == 0) {
			if (strcmp(token, "#include") == 0) {
				flagInclude = 1;
			}
			else {
				flagInclude = 0;
			}
		}
		if (i == 1) {
			if (token[0] == '\"' && token[strlen(token) - 1] == '\"') {
				flagOkFilename = 1;
			}
			else {
				flagOkFilename = 0;
			}
		}

		token = strtok(NULL, " ");
		i++;
	}
	if (stringg != NULL) {
		free(stringg);
		stringg = NULL;
	}
	return flagInclude && flagOkFilename;
	
}
int getSize(char* _string)
{
	char* t;
	int size = 0;

	for (t = _string; *t != '\0'; t++) {
		size++;
	}

	return size;
}
char* getIncludeFile(char* _string)
{
	const char delim[2] = " ";
	char* token;

	char* stringg = (char*)malloc(sizeof(char) * (strlen(_string) + 1));
	if (stringg == NULL) {
		printf("Can't allocate memory\n");
		exit(12);
	}
	strcpy(stringg, _string);

	token = strtok(stringg, delim);

	int i = 0;
	char* filename = (char*)malloc(sizeof(char) * 256);
	if (filename == NULL) {
		printf("Can't allocate memory\n");
		exit(12);
	}
	while (token != NULL) {
		if (i == 1) {
			strcpy(filename, token);
		}
		token = strtok(NULL, delim);
		i++;
	}
	if (filename[0] == '<' || filename[strlen(filename) - 1] == '>') {
			printf("Bad name! You can't include system libraries");
			exit(2);
	}

	char* newFileName = (char*)malloc(sizeof(char) * (strlen(filename) + 1));
	
	if (newFileName == NULL) {
		printf("No filename!\n");
		exit(12);
	}
	strcpy(newFileName, filename);
	newFileName++;

	newFileName[strlen(newFileName) - 1] = '\0';
	newFileName[strlen(newFileName) - 1] = '\0';
	
	if (stringg != NULL) {
		free(stringg);
		stringg = NULL;
	}
	if (filename != NULL) {
		free(filename);
		filename = NULL;
	}
	return newFileName;
}
