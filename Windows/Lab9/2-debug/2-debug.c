/* do not use UNICODE */
#undef _UNICODE
#undef UNICODE

#include <windows.h>
#include <stdio.h>

#include "utils.h"

#define	BUFSIZE		16

struct thread_info {
	char name[BUFSIZE];
};

/* Ingredients */
HANDLE Sugar;
HANDLE Milk;
HANDLE Eggs;
HANDLE Flour;

VOID create_ingredients(VOID);
DWORD WINAPI MakeCake(LPVOID lpParameter);
DWORD WINAPI MakeMarshmallows(LPVOID lpParameter);
DWORD WINAPI MakeTiramisu(LPVOID lpParameter);

HANDLE StartThread(LPTHREAD_START_ROUTINE ThreadFunc, LPVOID params)
{
	HANDLE hThread;

	/* TODO - Create Thread to executed ThreadFunc */
	hThread = CreateThread( //creez threadul care va executa functia ThreadFunc
		NULL,
		0,
		(LPTHREAD_START_ROUTINE)ThreadFunc,
		params,
		0,
		(int)0
	);
	/* TODO - Return Thread Handle */
	DIE(hThread == NULL, "Create Thread");
	return hThread;
}


int main(VOID)
{

	HANDLE ta, tb, tc;
	DWORD dwReturn;

	struct thread_info ti_a, ti_b, ti_c; //structurile ce vor tine minte infor despre threaduri

	setbuf(stdout, NULL);

	create_ingredients();

	/* Init parameters */
	sprintf_s(ti_a.name, sizeof(ti_a.name), "Chef A"); //initializez threadul ce va reprezenta cheful A
	sprintf_s(ti_b.name, sizeof(ti_b.name), "Chef B"); //initializez threadul ce va reprezenta cheful B
	sprintf_s(ti_c.name, sizeof(ti_c.name), "Chef C"); //initializez threadul ce va reprezenta cheful C

	/* Make food */ //initializez threadurile
	ta = StartThread(MakeCake, &ti_a); //primul va face prajitura
	tb = StartThread(MakeMarshmallows, &ti_b); //al doilea marhmallow
	tc = StartThread(MakeTiramisu, &ti_c); //al treilea tiramisu
	//ATENTIE - threadurile nu vor prepara in aceeasi ordine prajiturile dorite
	/* Wait for threads to finish */
	dwReturn = WaitForSingleObject(ta, INFINITE);
	DIE(dwReturn == WAIT_FAILED, "WaitForSingleObject");

	dwReturn = WaitForSingleObject(tb, INFINITE);
	DIE(dwReturn == WAIT_FAILED, "WaitForSingleObject");

	dwReturn = WaitForSingleObject(tc, INFINITE);
	DIE(dwReturn == WAIT_FAILED, "WaitForSingleObject");

	return 0;
}



DWORD WINAPI MakeCake(LPVOID lpParameter)
{
	DWORD dwRet;
	struct thread_info* ti = (struct thread_info*)lpParameter;

	printf("Chef %s wants to make Cake\n", ti->name);

	/* TODO - Fix the order in which the semaphores are decremented */
	dwRet = WaitForSingleObject(Sugar, INFINITE); 
	DIE(dwRet == WAIT_FAILED, "WaitForSingleObject");
	printf("Chef %s wants the sugar\n", ti->name);

	dwRet = WaitForSingleObject(Milk, INFINITE);
	DIE(dwRet == WAIT_FAILED, "WaitForSingleObject");
	printf("Chef %s wants the milk\n", ti->name);

	dwRet = WaitForSingleObject(Eggs, INFINITE);
	DIE(dwRet == WAIT_FAILED, "WaitForSingleObject");
	printf("Chef %s wants the eggs\n", ti->name);

	printf("Chef %s is making Cake\n", ti->name);
	Sleep(1);

	dwRet = ReleaseSemaphore(Eggs, 1, NULL);
	DIE(dwRet == 0, "ReleaseSemaphore");
	dwRet = ReleaseSemaphore(Milk, 1, NULL);
	DIE(dwRet == 0, "ReleaseSemaphore");
	dwRet = ReleaseSemaphore(Sugar, 1, NULL);
	DIE(dwRet == 0, "ReleaseSemaphore");

	printf("Chef %s finished!!!\n", ti->name);

	return 0;
}


DWORD WINAPI MakeTiramisu(LPVOID lpParameter)
{
	DWORD dwRet;
	struct thread_info* ti = (struct thread_info*)lpParameter;

	printf("Chef %s wants to make tiramisu\n", ti->name);

	/* TODO - Fix the order in which the semaphores are decremented */
	dwRet = WaitForSingleObject(Flour, INFINITE);
	DIE(dwRet == WAIT_FAILED, "WaitForSingleObject");
	printf("Chef %s wants the milk\n", ti->name);

	dwRet = WaitForSingleObject(Sugar, INFINITE);
	DIE(dwRet == WAIT_FAILED, "WaitForSingleObject");
	printf("Chef %s wants the sugar\n", ti->name);

	dwRet = WaitForSingleObject(Eggs, INFINITE);
	DIE(dwRet == WAIT_FAILED, "WaitForSingleObject");
	printf("Chef %s wants the eggs\n", ti->name);

	dwRet = WaitForSingleObject(Milk, INFINITE);
	DIE(dwRet == WAIT_FAILED, "WaitForSingleObject");
	printf("Chef %s wants the flour\n", ti->name);

	printf("Chef %s is making tiramisu\n", ti->name);
	Sleep(1);

	dwRet = ReleaseSemaphore(Flour, 1, NULL);
	DIE(dwRet == 0, "ReleaseSemaphore");
	dwRet = ReleaseSemaphore(Eggs, 1, NULL);
	DIE(dwRet == 0, "ReleaseSemaphore");
	dwRet = ReleaseSemaphore(Sugar, 1, NULL); //inloc milk cu sugar
	DIE(dwRet == 0, "ReleaseSemaphore");
	dwRet = ReleaseSemaphore(Milk, 1, NULL); //inloc sugar cu milk
	DIE(dwRet == 0, "ReleaseSemaphore");

	printf("Chef %s finished!!!", ti->name);

	return 0;
}


DWORD WINAPI MakeMarshmallows(LPVOID lpParameter)
{
	DWORD dwRet;
	struct thread_info* ti = (struct thread_info*)lpParameter;

	printf("Chef %s wants to make marshmallows\n", ti->name);

	dwRet = WaitForSingleObject(Flour, INFINITE);
	DIE(dwRet == WAIT_FAILED, "WaitForSingleObject");
	printf("Chef %s wants the flour\n", ti->name);
	Sleep(1);

	dwRet = WaitForSingleObject(Milk, INFINITE);
	DIE(dwRet == WAIT_FAILED, "WaitForSingleObject");
	printf("Chef %s wants the milk\n", ti->name);

	dwRet = WaitForSingleObject(Eggs, INFINITE);
	DIE(dwRet == WAIT_FAILED, "WaitForSingleObject");
	printf("Chef %s wants the eggs\n", ti->name);

	dwRet = WaitForSingleObject(Sugar, INFINITE);
	DIE(dwRet == WAIT_FAILED, "WaitForSingleObject");
	printf("Chef %s wants the sugar\n", ti->name);

	printf("Chef %s is making marshmallows\n", ti->name);
	Sleep(1);

	dwRet = ReleaseSemaphore(Sugar, 1, NULL); //inloc eggs cu sugar
	DIE(dwRet == 0, "ReleaseSemaphore");
	dwRet = ReleaseSemaphore(Eggs, 1, NULL); //inloc milk cu eggs
	DIE(dwRet == 0, "ReleaseSemaphore");
	dwRet = ReleaseSemaphore(Milk, 1, NULL); //inloc sugar cu milk
	DIE(dwRet == 0, "ReleaseSemaphore");
	dwRet = ReleaseSemaphore(Flour, 1, NULL);
	DIE(dwRet == 0, "ReleaseSemaphore");


	printf("Chef %s finished!!!\n", ti->name);

	return 0;
}



VOID create_ingredients(VOID)
{
	Sugar = CreateSemaphore(NULL, 1, 1, "Sugar"); //creez semafor pt zahar
	DIE(Sugar == NULL, "CreateSemaphore failed"); 

	Milk = CreateSemaphore(NULL, 1, 1, "Milk"); //creez semafor pt lapte
	DIE(Milk == NULL, "CreateSemaphore failed");

	Eggs = CreateSemaphore(NULL, 1, 1, "Eggs"); //creez semafor pt oua
	DIE(Eggs == NULL, "CreateSemaphore failed");

	Flour = CreateSemaphore(NULL, 1, 1, "Flour"); //creez semafor pt faina
	DIE(Flour == NULL, "CreateSemaphore failed");
}
