/*
 * Loader Implementation
 *
 * 2018, Operating Systems
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>

#include "exec_parser.h"

static so_exec_t *exec;
static int fd;
static int pageSize;

int min(int a, int b) {
	if (a < b)
		return a;
	else
		return b;
}

void mappingPage(uintptr_t pageFaultAddress, so_seg_t *segment)
{
	int nrPages;
	uintptr_t addr;
	int pageCnt;
	uintptr_t pageAddressForMapping;
	char *mappedAddress;
	int bytesRead;
	int mapRem;
	unsigned int fileSize;

	nrPages = segment->mem_size / pageSize;
	if (segment->data == NULL) {
		segment->data = (void *) malloc(nrPages * sizeof(char));
		memset(segment->data, 0, nrPages * sizeof(char));
	}
	addr = segment->vaddr;
	pageCnt = (pageFaultAddress - addr) / pageSize;
	pageAddressForMapping = pageCnt * pageSize;
	mappedAddress = mmap(
		(void *)(segment->vaddr + pageAddressForMapping), pageSize,
		PROT_WRITE, MAP_SHARED | MAP_FIXED | MAP_ANON, -1, 0); 
	if (mappedAddress == MAP_FAILED) {
		fprintf(stderr, "cannot mapping page");
		exit(-1);
	}
	fileSize = segment->file_size;
	if (pageAddressForMapping < fileSize) {
		lseek(fd, segment->offset + pageAddressForMapping, SEEK_SET);
		mapRem = fileSize - (unsigned int)pageAddressForMapping;
		bytesRead = read(fd, mappedAddress, min(mapRem, pageSize));
		if (bytesRead == -1) {
			fprintf(stderr, "cannot read from file\n");
			exit(-1);
		}
	}
	mprotect(mappedAddress, pageSize, PROT_NONE);
	if ((segment->perm & PERM_R) != 0)
		mprotect(mappedAddress, pageSize, PROT_READ);
	if ((segment->perm & PERM_W) != 0)
		mprotect(mappedAddress, pageSize, PROT_READ | PROT_WRITE);
	if ((segment->perm & PERM_X) != 0)
		mprotect(mappedAddress, pageSize, PROT_READ | PROT_WRITE | PROT_EXEC);
}

void segv_handler(int sig_no, siginfo_t *sig_info, void *context)
{
	uintptr_t pageFaultAddress;
	size_t i;
	so_seg_t *segment;
	uintptr_t maximumLimit;

	if (sig_info == NULL)
		exit(-1);
	pageFaultAddress = (uintptr_t)sig_info->si_addr;
	for (i = 0; i < exec->segments_no; i++) {
		segment = &exec->segments[i];
		maximumLimit = segment->vaddr + segment->mem_size;
		if (segment->vaddr <= pageFaultAddress && 
		    pageFaultAddress < maximumLimit) {
			mappingPage(pageFaultAddress, segment);
			return;
		}
	}
	fprintf(stderr, "Seg Fault\n");
	signal(SIGSEGV, NULL);
}

int so_init_loader(void)
{
	/* TODO: initialize on-demand loader */
	struct sigaction sa;
	int rc;

	pageSize = getpagesize();
	memset(&sa, 0, sizeof(struct sigaction));
	sa.sa_sigaction = segv_handler;
	rc = sigaction(SIGSEGV, &sa, NULL);
	if (rc < -1) {
		fprintf(stderr, "Cannot set sigaction\n");
		exit(-1);
	}
	return 0;
}

int so_execute(char *path, char *argv[])
{
	exec = so_parse_exec(path);
	if (!exec)
		return -1;
	fd = open(path, O_RDONLY, 0644);
	if (fd < 0) {
		fprintf(stderr, "cannot open file\n");
		exit(-1);
	}
	so_start_exec(exec, argv);
	return -1;
}
