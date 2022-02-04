/**
 * SO, 2014
 * Lab #6, Memoria virtuala
 *
 * Task #4, Windows
 *
 * Changing access right to pages
 */

/* do not use UNICODE */
#undef _UNICODE
#undef UNICODE

//#define _WIN32_WINNT 0x0500

#include <stdio.h>
#include <assert.h>
#include <limits.h>
#include <windows.h>

#include "utils.h"

static LPVOID access_violation_handler;
static int pageSize = 0x1000;
static LPBYTE p;
static int how[3] = { PAGE_NOACCESS, PAGE_READONLY, PAGE_READWRITE };

/*
 * SIGSEGV handler
 */
static LONG CALLBACK access_violation(PEXCEPTION_POINTERS ExceptionInfo)
{
	DWORD old, rc;
	LPBYTE addr;
	int pageNo;

	/* TODO 2 - get the memory location which caused the page fault */
	addr = (LPBYTE)ExceptionInfo->ExceptionRecord->ExceptionInformation[1];
	//adresa care a provocat page faultu
	/* TODO 2 - get the page number which caused the page fault */
	pageNo = (int)(addr - p) / pageSize; //indexul paginii care a provocat page faultul
	printf("Exception on page %d\n", pageNo);

	/* TODO 2 - test if page is one of our own */
	if (!(pageNo >= 0 || pageNo < 3)) { //daca nu se afla in intervalul celor 3 pagini, nu il tratez
		printf("wrong page");
		return EXCEPTION_CONTINUE_SEARCH;
	}

	/* TODO 2 - increase protection for that page */
	if (how[pageNo] == PAGE_NOACCESS) //daca n am nicio permisiune
	{
		how[pageNo] = PAGE_READONLY; //ii pun permisiune de read only
		rc = VirtualProtect(p + pageNo * pageSize, pageSize, PAGE_READONLY, &old); // setez noua permisiune
		DIE(rc == FALSE, "cannot virtual protect");
		printf("Give read only to page %d\n", pageNo);
	}
	else 
	{
		how[pageNo] = PAGE_READWRITE; // ii pun permisiune de read write
		rc = VirtualProtect(p + pageNo * pageSize, pageSize, PAGE_READWRITE, &old); // setez noua permisiune
		DIE(rc == FALSE, "cannot virtual protect");
		printf("Give read write to page %d\n", pageNo);

	}

	return EXCEPTION_CONTINUE_EXECUTION;
}

/*
 * sets SIGSEGV handler
 */
static void set_signal(void)
{
	/* TODO 2 - add VectoredHandler */
	access_violation_handler = AddVectoredExceptionHandler(1, access_violation);
	//adaug la lista functiilor de executat si access_violation,
	//care va trata semanlul de sigsegv
	DIE(access_violation_handler == NULL, "add vectored");
}

/*
 * restores SIGSEGV handler
 */
static void restore_signal(void)
{
	/* TODO 2 - remove VectoredHandler */

	int rc = RemoveVectoredExceptionHandler(access_violation_handler);
	//sterg functia callback adaugata care a tratat semnalul
	DIE(rc == FALSE, "remove vectored");
}

int main(void)
{
	BYTE ch;
	DWORD old, rc;
	BOOL bRet;
	/* TODO 1 - Map 3 pages with the desired memory protection
	 * Use global 'p' variable to keep the address return by VirtualAlloc
	 * Use VirtualProtect to set memory protection based on global 'how'
	 * array; 'how' array keeps protection level for each page
	 */
	p = VirtualAlloc( //mapez memorie virtuala
		NULL, // n am o adresa preferata
		3 * pageSize, //mapez 3 pagini
		MEM_COMMIT, //tipu operatiei, alocare completa, ptc folosesc commit
		PAGE_NOACCESS //paginile n au nicio permisiune
	);
	DIE(p == NULL, "cannot virtual alloc");

	bRet = VirtualProtect(p, pageSize, PAGE_NOACCESS, &old);
	//prima pagina n are nicio protectie
	DIE(bRet == FALSE, "cannot protect first page");
	bRet = VirtualProtect(p + pageSize, pageSize, PAGE_READONLY, &old);
	//a doua are protectie de read
	DIE(bRet == FALSE, "cannot protect second page");
	bRet = VirtualProtect(p + 2 * pageSize, pageSize, PAGE_READWRITE, &old);
	// a treia are protectie de read write
	DIE(bRet == FALSE, "cannot protect third page");
	set_signal(); //setez handle de semnale

	ch = p[0*pageSize]; p[0*pageSize] = 'a'; //fortez read + write la toate paginile
	ch = p[1*pageSize]; p[1*pageSize] = 'a';
	ch = p[2*pageSize]; p[2*pageSize] = 'a';

	restore_signal(); //restaurez vechiul handle de semnale

	/* TODO 1 - cleanup */
	rc = VirtualFree(p, 3 * pageSize, MEM_DECOMMIT); //demapez paginile mapate
	DIE(rc == FALSE, "virtual free");
	return 0;
}
