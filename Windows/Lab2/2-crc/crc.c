/**
 * SO, 2017
 * Lab #2, Operatii I/O simple
 *
 * Task #2, Windows
 *
 * Implementing simple crc method
 */

/* do not use UNICODE */
#undef _UNICODE
#undef UNICODE

#include <windows.h>
#include <stdio.h>

#include <utils.h>
#include <crc32.h>

#define BUFSIZE 512
#define CHUNKSIZE 32

static void WriteCrc(int crc, HANDLE hWrite)
{
	BOOL bRet;
	DWORD dwBytesWritten, dwBytesToWrite, dwTotalWritten;
	/* TODO 1 - Write the CRC to the file. Use a loop! */
	dwBytesToWrite = sizeof(int); //cat am de scris efectiv
	do {
		bRet = WriteFile(
			hWrite, //handle ul de scriere
			&crc, //inceputul locatiei de mem de scriere
			dwBytesToWrite, //cat tre sa scriu
			&dwBytesWritten, //cat am scris efectiv
			NULL //str overlapped
		);
		DIE(bRet == FALSE, "cannot write");
		dwTotalWritten += dwBytesWritten; //actualizez cat am scris
		dwBytesToWrite -= dwBytesWritten; //scad cat am scris
	} while(dwTotalWritten < dwBytesToWrite); 
}

static void GenerateCrc(CHAR *sourceFile, CHAR *destFile)
{
	HANDLE hRead, hWrite;
	CHAR buf[BUFSIZE];
	BOOL bRet;
	DWORD bytesRead;
	int crc = 0;

	/* TODO 1 - Open source file for reading */
	hRead = CreateFile(  //deschid fisieru pt citire
		sourceFile, //numele fisierului
		GENERIC_READ, //mod citire
		0, //niciun share mode
		NULL, //sec atr
		OPEN_EXISTING, //deschid doar daca exista
		FILE_ATTRIBUTE_NORMAL, //atrib normale
		NULL);
	DIE(hRead == INVALID_HANDLE_VALUE, "cannot create read file");
	/* TODO 1 - Create destination file for writing */
	hWrite = CreateFile ( //creez fisieru pt scriere
		destFile, //numele fisierului
		GENERIC_WRITE, //mod scriere
		0, //niciun share mode
		NULL, //sec atrr
		CREATE_NEW, //il si creeaza daca nu exista
		FILE_ATTRIBUTE_NORMAL, //atrib normale
		NULL);
	DIE(hWrite == INVALID_HANDLE_VALUE, "cannot create write file");
	/* Read from file  */
	while (1) {
		ZeroMemory(buf, sizeof(buf)); //zeroizez zona de memorie

		/* TODO 1 - Read from source file into buf BUFSIZE bytes */
		bRet = ReadFile ( 
			hRead, //handle ul de citire
			buf, //unde stochez ce citesc
			BUFSIZE, //cat tre sa citesc
			&bytesRead, //cat citesc efectiv
			NULL //str overlapped
		);
		DIE(bRet == FALSE, "cannot read");
		/* TODO 1 - Test for end of file */
		if(bytesRead == 0) {
			break; //daca nu mai am ce citi, ajung practic la final
		}
		/* Calculate crc for buf */
		crc = update_crc(crc, (unsigned char *) buf, bytesRead);
	}

	/* Write crc to destination file */
	WriteCrc(crc, hWrite); //scrie crc u in file destonation

	/* TODO 1 - Close files */
	 
	bRet = CloseHandle(hRead); //inchid handle ul de citire
	DIE(bRet == -1, "cannot close read file");
	bRet = CloseHandle(hWrite); //inchid handle ul de scriere
	DIE(bRet == -1, "cannot close write file");
}


static DWORD GetSize(HANDLE file)
{
	DWORD dwSize;

	/* TODO 2 - Calculate and return file size using SetFilePointer */
	dwSize = SetFilePointer( //vreau sa vad ce dimensiune are fisieru
		file, //handle ul
		0, //offsetu 0
		NULL, 
		FILE_END //unde sa mut, mut la final ca mi trebe sa vad ce dimensiune are fisieru
	);
	DIE(dwSize < 0, "is dis a file??");
	return dwSize;
}

static DWORD ReadChunk(CHAR *chunk, HANDLE hFile)
{
	BOOL bRet;
	DWORD dwBytesRead, dwBytesToRead, dwTotalRead = 0;

	/*
	 * TODO 3
	 * Read at most CHUNKSIZE bytes from file into the buffer. Use a loop!
	 * Return the number of read bytes.
	 */
	dwBytesToRead = CHUNKSIZE; //cat tre sa citesc 
	
	do
	{
		bRet = ReadFile(
			hFile, //handle
			chunk, //unde citesc
			dwBytesToRead, //cat tre sa citesc
			&dwBytesRead, //cat citesc efectiv
			NULL //str overlapped
		);
		DIE(bRet == FALSE, "cannot read");
		dwTotalRead += dwBytesRead; //actualizez cat am citit per total cu cat am citit tura asta
		dwBytesToRead -= dwBytesRead; //scad cat am citit tura asta
	}while(dwTotalRead < dwBytesToRead); //atata timp cand voi citit per total e mai mic decat am de citit
	return dwTotalRead;
}

static BOOL CompareFiles(CHAR *file1, CHAR *file2)
{
	DWORD bytesRead1, bytesRead2;
	HANDLE hFile1, hFile2;
	CHAR chunk1[CHUNKSIZE], chunk2[CHUNKSIZE];
	BOOL result = FALSE, bRet;

	/* TODO 4 - Open file handles */
	
	hFile1 = CreateFile( //creez fisier
		file1, //numele lui
		GENERIC_READ, //mod citire
		FILE_SHARE_READ, //mod shareuire pt citire
		NULL, //sec attr
		OPEN_EXISTING, //deschid doar daca exista 
		FILE_ATTRIBUTE_NORMAL, //atrib normale
		NULL		
	);
	DIE(hFile1 == INVALID_HANDLE_VALUE, "cannot open file1");
	
	hFile2 = CreateFile(
		file2, //numele fisierului
		GENERIC_READ, //mod citire
		FILE_SHARE_READ, //mod shareuire pt citire
		NULL, //sec atrr
		OPEN_EXISTING, //deschid doar daca exista
		FILE_ATTRIBUTE_NORMAL, //atrib normale
		NULL		
	);
	DIE(hFile1 == INVALID_HANDLE_VALUE, "cannot open file1");
	
	/* TODO 4 - Compare file size */
	if(GetSize(hFile1) == GetSize(hFile2)){ //daca am aceeasi dimensiune
		result = TRUE;
	}
	else {
		return result;
	}
	/* TODO 4 - Compare the files, chunk by chunk */
	while (1) {
		ZeroMemory(chunk1, sizeof(chunk1)); //zeroizez prima loc de memorie
	 	ZeroMemory(chunk2, sizeof(chunk2)); //zeroizez a doua loc de memorie

		bytesRead1 = ReadChunk(chunk1, hFile1); //vad cat am citit din prima
		bytesRead2 = ReadChunk(chunk2, hFile2); //vad cat am citit din a doua

		/* TODO 4 - Test for the end of the files */
		if(bytesRead1 == 0 && bytesRead2 == 0) //daca ajung la sfarsitu amandoura in acelasi timp
		{
			break;
		}
		/* TODO 4 - Compare the previously read chunks */
		for(int i = 0; i < bytesRead1; i++) {  
			if(chunk1[i] != chunk2[i]){ //verific daca am citit aceleasi chestii 
				result = FALSE;
				break;
			}
		}
	}

//exit:
	/* TODO 4 - Close files */
	bRet = CloseHandle(hFile1); //inchid handle
	DIE(bRet == FALSE, "cannot close file1");
	bRet = CloseHandle(hFile2); //inchid handle
	DIE(bRet == FALSE, "cannot close file2");
	return result;
}

int main(int argc, char *argv[])
{
	BOOL equal;

	if (argc != 4) {
		fprintf(stderr, "Usage:\n"
				"\tcrc.exe -g <input_file> <output_file> - generate crc\n"
				"\tcrc.exe -c <file1> <file2>            - compare files\n");
		exit(EXIT_FAILURE);
	}

	if (strcmp(argv[1], "-g") == 0)
		GenerateCrc(argv[2], argv[3]);

	if (strcmp(argv[1], "-c") == 0) {
		equal = CompareFiles(argv[2], argv[3]);

		if (equal)
			printf("Files are equal\n");
		else
			printf("Files differ\n");
	}

	return 0;
}
