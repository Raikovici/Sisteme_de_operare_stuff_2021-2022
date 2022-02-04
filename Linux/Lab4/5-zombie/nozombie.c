/**
  * SO, 2016
  * Lab #4
  *
  * Task #5, lin
  *
  * Avoid creating zombies using signals
  */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <signal.h>
#include <sys/types.h>
#include <unistd.h>

#include "utils.h"

#define TIMEOUT		20


/*
 * configure signal handler
 */
static void set_signals(void)
{
	struct sigaction sa;
	int rc;
	memset(&sa, 0, sizeof(sa));
	sa.sa_flags = SIG_IGN; //flagul de ignore
	rc = sigaction(SIGCHLD, &sa, NULL); //tratarea semnalului
	//SIGCHLD e trimis parintelui atunci cand executia copilului se termina
	//ignorandu l, practic parintele nu asteapta dp copil sa se incheie
	DIE(rc < 0, "cannot ignore");
	/* TODO - ignore SIGCHLD */

}

int main(void)
{
	pid_t pid;
	set_signals(); //functie care va ignora asteptarea copilului
	/* TODO - create child process without waiting */
	pid = fork();
	switch(pid) {
		case -1:
			exit(-1);
			break;
		case 0:
			exit(EXIT_SUCCESS);
			break;
		default:
			break;
	}
	/* TODO - sleep */
	sleep(TIMEOUT);
	return 0;
}
