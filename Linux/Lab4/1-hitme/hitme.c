/**
  * SO, 2016
  * Lab #4
  *
  * Task #1, lin
  *
  * Catching signals
  */
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

#include "utils.h"

static int must_leave;
void *mem;
static int signals_to_mask[] =
           { SIGSEGV, SIGTSTP, SIGTTIN, SIGTTOU, SIGURG, SIGXCPU, SIGXFSZ }; //arrayu de semnale care va fi trimis

static void sig_handler(int signum)
{
	switch (signum) {
	case SIGSEGV: //daca e sigsegv ii zic ca nu e bine sa accesezi mem neinitializata
		printf("No! Do NOT access bad memory! Bad programmer!\n");
		break;
	case SIGTSTP: // semnal care spre deosebire de SIGSTOP, permite sa fie handled
		printf("H");
		break;
	case SIGTTIN: // semnal care permite citirea unui background job in terminal
		printf("e");
		break;
	case SIGTTOU: //similar cu SIGTTIN dar e vb de scriere
		printf("l");
		break;
	case SIGURG: //semnal care trimite flag de URG pe un socket
		printf("l");
		break;
	case SIGXCPU: //semnal care e reprez de cpu time exceeded error
		printf("o");
		break;
	case SIGXFSZ: //semnal care e reprez de file size limit exceeded error
		printf("\n"); 
		must_leave = 1;
		break;
	default:
		printf("Go back and send the following signals: "
				"20, 21, 22, 23, 24");
		must_leave = 1;
	}
	fflush(stdout);
}

int main(void)
{
	struct sigaction signals; //structura folosita pt tratarea semnalelor
	void *mem = NULL;
	sigset_t mask;
	unsigned int i;
	int rc;

	sigemptyset(&mask); //initial pornesc cu masca de semnale zeroizata
	

	memset(&signals, 0, sizeof(struct sigaction));
	//signals.sa_flags = SA_RESETHAND; //daca e activata, va restaura handlerul la state ul anterior
	signals.sa_mask = mask; //salvez noua masca de semnale

	signals.sa_handler = sig_handler; //initialize membrul sa_handler al structurii signals, cu pointerul functiei de tratare a semanlelor

	for (i = 0; i < sizeof(signals_to_mask)/sizeof(int); i++) {
		rc = sigaction(signals_to_mask[i], &signals, NULL); //asociez pt fiecare semnal din arrayu de semnale, structura signals pt tratarea semnalelor
		DIE(rc == -1, "sigaction");
	}

	/* TODO 2: This is a very rare opportunity to get creative! What's
	 * even more rare: it's something you're usually advised against
	 * doing. Below, implement a means to access an invalid memory area
	 * and cause a "Segmentation fault". When choosing your approach,
	 * choose the most elegant.
	 */
	 
	 char* q = (char*)mem;
	 printf("%s\n", q); //asta va trimite semnalul SIGSEGV ptc accesez locatia mem care nu e alocata inca


	while (!must_leave)
		sleep(1);

	return 0;
}
