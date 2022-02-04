/**
  * SO, 2016
  * Lab #4
  *
  * Task #5, lin
  *
  * Creating zombies
  */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <signal.h>
#include <sys/types.h>
#include <unistd.h>

#include "utils.h"

#define TIMEOUT		20


int main(void)
{
	pid_t pid;

	/* TODO - create child process without waiting */
	pid = fork(); //nou proces
	switch(pid) {
		case -1:
			break; //error
		case 0:
			exit(EXIT_SUCCESS); //copilu isi termina executia
			break;
		default: 
			break; //parintele nu asteapta terminarea executiei copilului
	}
	/* TODO - sleep */
	sleep(TIMEOUT);
	return 0;
}
