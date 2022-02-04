/**
 * SO, 2014
 * Lab #6, Memoria virtuala
 *
 * Task #1, Windows
 */

/* do not use UNICODE */
#undef _UNICODE
#undef UNICODE

#include <stdio.h>
#include <assert.h>
#include <limits.h>
#include <windows.h>

#include "utils.h"


static void wait_for_input(const char *msg)
{
	char buf[32];

	printf(" * %s\n", msg);
	printf(" -- Press ENTER to continue ...\n"); fflush(stdout);
	fgets(buf, 32, stdin);
}

HANDLE open(const char *filename, DWORD flag)
{
	HANDLE hFile;

	hFile = CreateFile( //creez fisierul
			filename,
			FILE_READ_DATA | FILE_WRITE_DATA,
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

	hFileMap = CreateFileMapping( //creez un handle de tip file mapping
			fd, //handle ul fisierului
			NULL, //atr sec
			PAGE_READWRITE, //protectia, read + write
			0, //size high
			size, //size low
			NULL); //niciun nume special
	DIE(hFileMap == NULL, "CreateFileMapping");

	p = MapViewOfFile( //mapez in spatiul de adresa al procesului
			hFileMap, // handle ul creat de functia de mai sus
			FILE_MAP_ALL_ACCESS, //primeste toate tipurile de acces
			0, //size high
			0, //size low
			0); //cat sa mapeze
	DIE(p == NULL, "MapViewOfFile");

	return p;
}

int main(void)
{
	DWORD rc, dwPageSize;
	HANDLE hFile;
	char *mem;
	SYSTEM_INFO siSysInfo;

	GetSystemInfo(&siSysInfo);
	dwPageSize = siSysInfo.dwPageSize;

	wait_for_input("before mapping file ");

	hFile = open("test.txt", OPEN_EXISTING);
	mem = map(hFile, dwPageSize);

	wait_for_input("after mapping file ");

	printf("%c\n", mem[0]);

	wait_for_input("after reading from memory");

	rc = UnmapViewOfFile(mem);
	DIE(rc == FALSE, "UnmapViewOfFile");
	rc = CloseHandle(hFile);
	DIE(rc == FALSE, "CloseHandle");

	wait_for_input("closing mapped file");

	mem = VirtualAlloc( //aloc memorie virtuala
			NULL,			// System selects address
			dwPageSize,		// Size of allocation
			MEM_COMMIT,		// Allocate reserved pages
			PAGE_READWRITE);	// Protection
	DIE(mem == NULL, "VirtualAlloc");

	wait_for_input("mapped private memory");

	mem[0] = 'a';

	wait_for_input("after writing to memory");

	rc = VirtualFree(mem, 0, MEM_RELEASE);

	wait_for_input("after freeing memory");

	return 0;
}
