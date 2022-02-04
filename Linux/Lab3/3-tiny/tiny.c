/**
  * SO, 2017
  * Lab #3
  *
  * Task #3, lin
  *
  * Implementing a minimal shell
  */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>

#include "utils.h"

#define MAX_LINE_SIZE		256
#define MAX_ARGS		8

#define ERROR			0
#define SIMPLE			1
#define REDIRECT		2
#define PIPE			3
#define SET_VAR			4
#define SHELL_EXIT		5

static char *verb;
static char **args;
static char *stdin_file;
static char *stdout_file;
static char *stderr_file;

static char *value;
static char *var;

static int type;

static int parse_line(char *line);
static void alloc_mem(void);
static void free_mem(void);

/*
 * @filedes  - file descriptor to be redirected
 * @filename - filename used for redirection
 */
static void do_redirect(int filedes, const char *filename)
{
	int rc;
	int fd;

	/* TODO 3 - Redirect filedes into fd representing filename */
	fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644); //deschid fisieru in mod read write, il si creeaza daca nu exista, il truncheaza daca scrie in el
	DIE(fd == -1, "cannot create file");

	rc = dup2(fd, filedes); //duplic descriptorul dat ca parametru peste cel creat deja
	DIE(rc == -1, "cannot duplicate");
}

static void set_var(const char *name, const char *value)
{
	/* TODO 2 - Set the environment variable */
	setenv(name, value, 1); //setez variabila de mediu
}

static char *expand(const char *name)
{
	/* TODO 2 - Return the value of environment variables */
	return getenv(name); //returnez valoarea variabilei
}

/*
 *  @args - array that contains a simple command with parrameters
 */
static void simple_cmd(char **args)
{
	pid_t pid, wait_ret;
	int status;

	/* TODO 1 - Create a process to execute the command */
	pid = fork(); //creez un nou proces

	switch (pid) {
	case -1:
		/* TODO 1 - error */
		fprintf(stderr, "Error, cannot create process\n");
		break;
	case 0:
		/* redirect standard output if needed */
		if (stdout_file != NULL) //daca nu am neaparat standard output file
			do_redirect(STDOUT_FILENO, stdout_file); //redirectez outputu

		/* TODO 1 - child process */
		execvp(args[0], (char *const *)args); //execut practic <comanda> <arg> <arg>....
		break;
	default:
		/* TODO 1 -  parent process */
		wait_ret = waitpid(pid, &status, 0); //astept sa se incheie exec copilului
		DIE(wait_ret < 0, "waitpid");
		break;
	}


}

int main(int argc, char** argv, char** environ)
{
	char line[MAX_LINE_SIZE];

	alloc_mem();

	while (1) {
		printf("> ");
		fflush(stdout);

		memset(line, 0, MAX_LINE_SIZE);

		if (fgets(line, sizeof(line), stdin) == NULL) {
			free_mem();
			exit(EXIT_SUCCESS);
		}

		type = parse_line(line);

		switch (type) {
		case SHELL_EXIT:
			free_mem();
			exit(EXIT_SUCCESS);

		case SET_VAR:
			set_var(var, value);
			break;

		case SIMPLE:
			simple_cmd(args);
			break;
		}
	}

	return 0;
}

static void alloc_mem(void)
{
	args = malloc(MAX_ARGS * sizeof(char *));
	DIE(args == NULL, "malloc");
}

static void free_mem(void)
{
	free(args);
}

static int parse_line(char *line)
{
	int ret = SIMPLE, idx = 0;
	char *token, *delim = "=\n";

	stdin_file = NULL;
	stdout_file = NULL;
	stderr_file = NULL;

	/* check for exit */
	if (strncmp("exit", line, strlen("exit")) == 0)
		return SHELL_EXIT;

	/* var = value */
	if (strchr(line, '=') != 0) {
		token = strtok(line, delim);

		if (token == NULL)
			return ERROR;

		var = strdup(token);

		token = strtok(NULL, delim);
		if (token == NULL)
			return ERROR;

		value = strdup(token);
		return SET_VAR;
	}

	/* normal command */
	delim = " \t\n";
	token = strtok(line, delim);

	if (token == NULL)
		return ERROR;

	verb = strdup(token);

	/* copy args */
	while (token != NULL) {
		if (token[0] == '$')
			token = expand(token+1);

		if (token == NULL) {
			printf(" Expansion failed\n");
			return ERROR;
		}

		if (strncmp(token, ">", strlen(">")) == 0) {
			if (strlen(token) > strlen(">")) {
				token++;
				stdout_file = strdup(token);
			} else {
				token = strtok(NULL, delim);
				stdout_file = strdup(token);
			}
		} else {
			args[idx++] = strdup(token);
		}

		token = strtok(NULL, delim);
	}

	args[idx++] = NULL;
	return ret;
}
