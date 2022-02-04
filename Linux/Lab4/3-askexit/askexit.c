/**
  * SO, 2016
  * Lab #4
  *
  * Task #3, lin
  *
  * Signal handling by asking questions
  */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include <signal.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#include "utils.h"

#define MY_MAX		32
#define TIMEOUT		1

static void print_next(void)
{
	static int n = 1;

	printf("n = %d\n", n);

	n = (n + 1) % MY_MAX;
}

/* signal handler */
static void ask_handler(int signo)
{
	char buffer[128];

	printf("Got %d  - Stop program? [Y/n] ", signo);
	fflush(stdout);
	fgets(buffer, 128, stdin);
	buffer[strlen(buffer)-1] = '\0';

	if (buffer[0] == 'y' || buffer[0] == 'Y')
		exit(EXIT_SUCCESS);
}

/* configure handlers */
static void set_signals(void)
{
	struct sigaction sa; //structura de tratare a semnalelor
	int rc;
	memset(&sa, 0, sizeof(sa)); //initializarea ei
	/* TODO - set handler in struct sigaction sa */
	sa.sa_handler = ask_handler; //functia care va trata semnalul
	/* TODO - handle SIGINT, SIGQUIT and SIGUSR1 signals */
	rc = sigaction(SIGINT, &sa, NULL); //trimit SIGINT
	DIE(rc < 0, "cannot sigint");
	rc = sigaction(SIGQUIT, &sa, NULL); // trimit SIGQUIT
	DIE(rc < 0, "cannot sigquit");
	rc = sigaction(SIGUSR1, &sa, NULL); //trimit SIGUSR1
	DIE(rc < 0, "cannot sigusr1");
}

int main(void)
{
	set_signals();

	while (1) {
		print_next();
		sleep(TIMEOUT);
	}

	return 0;
}
