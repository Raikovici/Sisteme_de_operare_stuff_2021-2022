/**
 * SO, 2017
 * Lab #2, Operatii I/O simple
 *
 * Task #1, Windows
 *
 * Implementing cat
 */

/* do not use UNICODE */
#undef _UNICODE
#undef UNICODE

#include <windows.h>
#include <stdio.h>

#include <utils.h>

#define BUFSIZE 8

int main(int argc, char *argv[])
{
	HANDLE hFile;
	CHAR buf[BUFSIZE];
	BOOL bRet;
	DWORD bytesRead, bytesWritten, totalWritten = 0;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s <file>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	/* open file with name pointed by argv[1] */
	hFile = CreateFile(argv[1],
			GENERIC_READ, //pentru citire
			0, //nicio optiune de sharing
			NULL, //security attr
			OPEN_EXISTING, //deschid daca exista
			FILE_ATTRIBUTE_NORMAL, //atributii normale
			NULL); //n am template
	DIE(hFile == INVALID_HANDLE_VALUE, "CreateFile argv[0]");

	/* read file pointed by argv[1] and send it to standadard output */
	while (1) {
		bRet = ReadFile(hFile, //handle ul de unde citesc
						buf, //unde stochez ce citesc
						BUFSIZE, //cat tre sa citesc
						&bytesRead, //cat citesc de fapt
						NULL); // str overlapped n am trb acu
		DIE(bRet == FALSE, "ReadFile"); 

		if (bytesRead == 0) //daca n am ce citi, rup bucla
			break;

		/* how much we've actually written */
		totalWritten = 0; //cat voi fi citit per total
		do {
			bRet = WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), //scriu la stdout
					buf + totalWritten, // locatia de mem de unde citesc, adica ma mut in bufferu care a stocat ce am citit
					bytesRead - totalWritten, // cat tre sa scriu, adica bytes cat am citit in prealabil minus cat am scris in total
					&bytesWritten, //cat am scris efectiv
					NULL); //str overlapped n am trb
			DIE(bRet == FALSE, "WriteFile"); 
			totalWritten += bytesWritten; //adaug cat am scris tura asta
		} while (totalWritten < bytesRead); // cat timp cat am scris total e mai mic decat cat am de scris efectiv
	}

	bRet = CloseHandle(hFile); //inchid handle ul
	DIE(bRet == FALSE, "CloseHandle");

	return 0;
}
