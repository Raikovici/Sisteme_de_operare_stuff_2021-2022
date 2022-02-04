/**
 * SO, 2017
 * Lab #2, Operatii I/O simple
 *
 * Task #1, Linux
 *
 * Redirection example with "dup" and "dup2"
 * and use of "lsof"
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "utils.h"

static void wait_for_input(const char *msg)
{
	char buf[32];

	printf(" * %s\n", msg);
	printf(" -- Press ENTER to continue ..."); fflush(stdout);
	fgets(buf, 32, stdin);
}


int main(void)
{
	int fd1, fd2, rc;

	wait_for_input("beginning");

	/* open files */
	fd1 = open("tmp1.txt", O_CREAT | O_RDWR, 0644); //deschid fisieru in mod read write, cu permisiuni RWX, se mai creeaza un descriptor pe pozitia 3
	DIE(fd1 < 0, "open tmp1.txt"); 
	wait_for_input("created tmp1.txt");

	fd2 = open("Makefile", O_RDONLY); //deschid fisieru makefile in mod read cu permisiuni RWX, se mai creeaza un descriptor pe pozitia 4
	DIE(fd2 < 0, "open Makefile");
	wait_for_input("opened Makefile");

	/* redirect stderr to fd1 */
	rc = close(STDERR_FILENO); //inchid descriptoru care pointeaza catre stderr, practic vom avea casuta 2 libera dar vin ceilalti descriptori si vom avea stdin,stdout,fd1,fd2
	DIE(rc < 0, "close stderr");
	wait_for_input("closed stderr");

	rc = dup(fd1); //duplic descriptoru 1, el va fi pe pozitia 4 in tabela
	DIE(rc < 0, "dup fd1");

	/* write something to stderr */
	write(STDERR_FILENO, "something", strlen("something")); //scriu in 
	wait_for_input("dup - redirected stderr to fd1. Written "
                       "something to STDERR_FILE. Inspect the associated file.");

	rc = close(fd1); //inchid descriptoru 1, ramanem cu 4 descr: stdin, stdout, copia lui fd1 si makefile
	DIE(rc < 0, "close fd1");
	wait_for_input("closed fd1");

	/* redirect stderr to fd2 */
	rc = dup2(fd2, STDERR_FILENO); //se duplica fd2 pe pozitia 2 unde ar trb sa fie tmp1
	DIE(rc < 0, "dup2 fd2");
	wait_for_input("dup2 - redirected stderr to fd2");

	rc = close(fd2); //se inchide fd2, ramanem cu 3 descriptori: stdin, stdout si fd2 pt stderr
	DIE(rc < 0, "close fd2");
	wait_for_input("closed fd2");

	return 0;
}
