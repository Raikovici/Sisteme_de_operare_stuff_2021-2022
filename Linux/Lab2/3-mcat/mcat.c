/**
 * SO, 2017
 * Lab #2, Operatii I/O simple
 *
 * Task #3, Linux
 *
 * cat/cp applications
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

#include "utils.h"
#include "xfile.h"

#define BUFSIZE		32

int main(int argc, char **argv)
{
	int fd_src, fd_dst, rc, bytesRead, bytesWritten, bytesToWrite, bytesTotalWritten;
	char buffer[BUFSIZE];

	if (argc < 2 || argc > 3) {
		printf("Usage:\n\t%s source_file [destination_file]\n",
			argv[0]);
		return 0;
	}

	/* TODO 1 - open source file for reading */
	fd_src = open(argv[1], O_CREAT | O_RDONLY, 0644); //deschid fisieru in mod read
	DIE(fd_src < 0, "cannot open file");

	if (argc == 3) {
		/* TODO 2 - redirect stdout to destination file */
		fd_dst = open(argv[2], O_CREAT | O_WRONLY | O_TRUNC, 0644); //deschid fisieru destinatie pt scriere
		DIE(fd_dst < 0, "cannot open file");
		dup2(fd_dst, STDOUT_FILENO); //duplic file descriptoru peste stdout, outputu va fi practic fisieru
		
	}

	/* TODO 1 - read from file and print to stdout
	 * use _only_ read and write functions
     * for writing to output use write(STDOUT_FILENO, buffer, bytesRead);
	 */
	memset(buffer, 0, sizeof(buffer)); //initializez bufferu

    /* TODO 3 - Change the I/O strategy and implement xread/xwrite. These
     * functions attempt to read _exactly_ the size provided as parameter.
     */
	while(1) {
		bytesRead = read(fd_src, buffer, BUFSIZE); //cat am citit, bytesRead
		if(bytesRead == 0) //daca i 0, n am citit nimic, ies din bucla
	 	{
			break;
	 	}
	 	bytesToWrite = bytesRead; //atat am de scris, adica atatia bytes cat am citit
	 	bytesTotalWritten = 0; //initializez nr total cat ar trb sa am la final nr total scriti

	 	while(bytesTotalWritten < bytesToWrite) //cat timp nr de bytes total scriti e mai mic decat cat am de scris 
	 	{
	 		bytesWritten = write(fd_dst, buffer + bytesTotalWritten, bytesToWrite); //scriu in fisieru destinatie
	 											   //ma mut si in buffer cu cat am citit pana atunci
	 		DIE(bytesWritten < 0, "cannot write"); //daca nu am ce scrie moare
	 		bytesTotalWritten += bytesWritten; //adaug la total cat am scris tura asta
	 		bytesToWrite -= bytesWritten; // scad cat am scris sa vad cati bytes mai am de scris efectiv
	 	}
	}

	/* TODO 1 - close file */
	rc = close(fd_src); //inchid fisieru destinatie
	DIE(rc < 0, "cannot close");
	return 0;
}
