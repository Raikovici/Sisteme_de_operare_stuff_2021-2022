/**
  * SO, 2017
  * Lab #3
  *
  * Task #4, lin
  *
  * FIFO server
  */
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

#include "utils.h"
#include "common.h"	/* PIPE_NAME, BUFSIZE */

int main(void)
{
	int fd, bytesRead, rc, offset = 0;
	char buff[BUFSIZE];

	/* TODO - create named pipe */
	fd = mkfifo(PIPE_NAME, O_CREAT | O_EXCL | S_IRWXU);
	DIE(fd == -1, "cannot create pipe");
	/* TODO - open named pipe */
	fd = open(PIPE_NAME, O_RDONLY);
	DIE(fd == -1, "cannot open pipe");
	/* TODO - read in buff from pipe while not EOF */
	memset(buff, 0, sizeof(buff));
	while(1) {

		bytesRead = read(fd, buff, BUFSIZE);
		if(bytesRead == 0)
			break;
	}
	printf("Message received:%s\n", buff);

	/* Close and delete pipe */
	rc = close(fd);
	DIE(rc < 0, "close");

	rc = unlink(PIPE_NAME);
	DIE(rc < 0, "unlink");

	return 0;
}
