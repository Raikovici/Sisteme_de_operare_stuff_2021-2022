/**
 * SO, 2014
 * Lab #6, Memoria virtuala
 *
 * Task #3, Windows
 *
 * Implementing copy using mapping
 */

/* do not use UNICODE */
#undef _UNICODE
#undef UNICODE

#include <stdio.h>
#include <assert.h>
#include <limits.h>
#include <windows.h>

#include "utils.h"

static int pageSize = 0x1000;

HANDLE open(const char *filename, DWORD flag)
{
	HANDLE hFile;

	hFile = CreateFile( //deschid fisieru
			filename, //cu acest nume
			FILE_READ_DATA | FILE_WRITE_DATA, //cu permisiuni de read write
			FILE_SHARE_READ, 
			NULL,
			flag,
			FILE_ATTRIBUTE_NORMAL,
			NULL);
	DIE(hFile == INVALID_HANDLE_VALUE, "CreateFile");
	return hFile;
}

LPVOID map(HANDLE fd, DWORD size)
{
	HANDLE hFileMap;
	LPVOID p;

	/* TODO map file */
	hFileMap = CreateFileMapping( //creez un handle de tip file mapping
		fd, //indicat de acest handle de fisier
		NULL, //sec atrr
		PAGE_READWRITE, //permisiuni de read write
		0, // size high
		size, //size low
		NULL //niciuun nume specific
	);
	DIE(hFileMap == NULL, "create file mapping");
	p = MapViewOfFile( //mapez in spatiu de adrese
		hFileMap, //handle ul returnat mai sus
		FILE_MAP_ALL_ACCESS, //toate tipurile de access
		0,
		0,
		0
	);
	DIE(p == NULL, "map view of file");
	return p;
}

int main(int argc, char **argv)
{
	HANDLE hSrc, hDst;
	void *pin, *pout;
	WIN32_FILE_ATTRIBUTE_DATA fileAttr;
	DWORD size, rc;
	BOOL bRet;

	DIE(argc != 3, "Usage:\n\tcopy <from_file> <to_file>");
	rc = GetFileAttributesEx(argv[1], GetFileExInfoStandard, &fileAttr);
	DIE(rc == FALSE, "GetFileAttributesEx");
	size = fileAttr.nFileSizeLow;

	/* opening files */
	hSrc = open(argv[1], OPEN_EXISTING); //primu fisier pt citire
	hDst = open(argv[2], CREATE_ALWAYS); // al doilea fisier pt scriere

	/* TODO - truncate the output file to the input file size */
	rc = SetFilePointer(hDst, size, NULL, FILE_BEGIN); // folosesc set file pointer
	//ptr trunchierea fisierului destinatie
	DIE(rc == -1, "set file pointer");
	bRet = SetEndOfFile(hDst); 
	/* TODO - map files and copy content */
	pin = map(hSrc, size); //mapez fisierul input
	pout = map(hDst, size); //mapez fisierul output
	memcpy(pout, pin, size); // copiez in locatia de mem pt out
	/* TODO - cleanup */
	bRet = UnmapViewOfFile(pin); //demapez locatia de mem unde am alocat fis de input
	DIE(bRet == FALSE, "cannot unmap pin");
	bRet = UnmapViewOfFile(pout);  //demapez locatia de mem unde am alocat fis de output
	DIE(bRet == FALSE, "cannot unmap pout");
	CloseHandle(hSrc);
	CloseHandle(hDst);
	return 0;
}
