/**
 * SO, 2017
 * Lab #6
 *
 * Task #5, lin
 *
 * Changing page access protection
 */
#include <signal.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

#include "utils.h"

static int pageSize;
static struct sigaction old_action;
char *p;
int how[3] = { PROT_NONE, PROT_READ, PROT_WRITE };

static void segv_handler(int signum, siginfo_t *info, void *context)
{
	char *addr;
	int rc;
	int pageIndex, prot;
	/* Calling the old signal handler by default for TODO 1 */
	/* Comment this line when solving TODO 2 */
	//old_action.sa_sigaction(signum, info, context);

	/* TODO 2 - check if the signal is SIGSEGV */
	if(info->si_signo != SIGSEGV) //daca semnalu trimis nu i SIGSEGV
	{
		old_action.sa_sigaction(signum, info, context); //restaurez vechiul handler
	}
	/* TODO 2 - obtain from siginfo_t the memory location
	 * which caused the page fault
	 */
	addr = info->si_addr; //obtin adresa care a cauzat page faultul
	if(addr < p || addr >= p + 3 * pageSize) //daca nu se afla in intervalul [p, p + 2*pagesize], n am treaba sa o tratez
	{
		old_action.sa_sigaction(signum, info, context);
	}

	/* TODO 2 - obtain the page which caused the page fault
	 * Hint: use the address returned by mmap
	 */
	pageIndex = (addr - p) / pageSize; //obtin indexul paginii care a cauzat page faultul
	/* TODO 2 - increase protection for that page */
	if(how[pageIndex] == PROT_NONE) //daca i none
	{
		how[pageIndex] = PROT_READ; //o pun pe read
		rc = mprotect(p + pageIndex * pageSize, pageSize, PROT_READ); //schimb protectia
		DIE(rc < 0, "cannot set read protection");
		printf("give read permission to %d page\n", pageIndex);
	}
	else //daca protectia e deja read, o pun pe write
	{
		how[pageIndex] = PROT_WRITE; 
		rc = mprotect(p + pageIndex * pageSize, pageSize, PROT_WRITE);
		DIE(rc < 0, "cannot set write protection");
		printf("give write permission to %d page\n", pageIndex);
	}
}

static void set_signal(void)
{
	struct sigaction action; //structura pt tratarea semnalelor
	int rc;

	action.sa_sigaction = segv_handler; //functia pt tratarea semnalelor
	sigemptyset(&action.sa_mask); //zeroizarea mastii de semnale
	sigaddset(&action.sa_mask, SIGSEGV); //adaugarea semnalului SIGSEGV
	
	action.sa_flags = SA_SIGINFO; //daca folosesc acest flag, indic structurii sa nu mai foloseasca un handler normal, ci o functie de tip sa_sigaction, care foloseste si structura siginfo_t, cu informatii speciale despre ce a cauzat page fault ul

	rc = sigaction(SIGSEGV, &action, &old_action);
	DIE(rc == -1, "sigaction");
}

static void restore_signal(void)
{
	struct sigaction action;
	int rc;

	action.sa_sigaction = old_action.sa_sigaction; //resturez vechiul handler de semnale
	sigemptyset(&action.sa_mask);
	sigaddset(&action.sa_mask, SIGSEGV);
	action.sa_flags = SA_SIGINFO;

	rc = sigaction(SIGSEGV, &action, NULL);
	DIE(rc == -1, "sigaction");
}

int main(void)
{
	int rc;
	char ch;

	pageSize = getpagesize(); //dimensiunea unei pagini

	/* TODO 1 - Map 3 pages with the desired memory protection
	 * Use global 'p' variable to keep the address returned by mmap
	 * Use mprotect to set memory protections based on global 'how' array
	 * 'how' array keeps protection level for each page
	 */
	p = mmap(NULL, 3 * pageSize, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0); //mapez o locatie de memorie care va avea ca fi cat 3 pagini
	DIE(p == MAP_FAILED, "cannot mmap");
	for(int i = 0; i < sizeof(how) / sizeof(*how); i++)
	{
		rc = mprotect(p + i * pageSize, pageSize, how[i]); //adaug pt fiecare pagina mapata protectie cf array ului care contine protectiile 
		DIE(rc < 0, "mprotect");
	}
	set_signal(); //setez handlerul de semnale
	
	
	/* TODO 1 - Access these pages for read and write */
	
	for(int i = 0; i < sizeof(how) / sizeof(*how); i++) {
		ch = 'a';
		if(how[i] == PROT_NONE) 
		{
			fprintf(stderr, "None access\n");
		}
		else if(how[i] == PROT_READ) 
		{
			*(p + i * pageSize) = ch; //fortez citirea
			fprintf(stderr, "Read access\n");
		}
		else if( how[i] == PROT_WRITE)
		{
			ch = *(p + i * pageSize); //fortez scrierea
			fprintf(stderr, "write access\n");
		}
		
	}
	restore_signal(); //restaurez handlerul vechi

	/* TODO 1 - unmap */
	rc = munmap(p, 3 * pageSize); //demapez
	DIE(rc < 0, "cannot munmap");


	return 0;
}
