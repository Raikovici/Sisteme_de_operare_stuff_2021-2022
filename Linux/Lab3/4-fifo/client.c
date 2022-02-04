/**
  * SO, 2017
  * Lab #3
  *
  * Task #4, lin
  *
  * FIFO client
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
	int fd, rc, bytesWritten, len, total = 0;
	char message[BUFSIZE];

	/* TODO - open named pipe for writing */
	fd = open(PIPE_NAME, O_WRONLY | O_CREAT | O_NONBLOCK);
	DIE(fd == -1, "cannot open pipe");
	/* Read message from user */
	memset(message, 0, sizeof(message));
	printf("Message to send:");
	scanf("%s", message);
	len = strlen(message);

	bytesWritten = write(fd, message, len);

	/* TODO - write message to pipe */

	/* close pipe */
	rc = close(fd);
	DIE(rc < 0, "close");

	return 0;
}
