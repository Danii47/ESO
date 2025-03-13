#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <ctype.h>

// Represents a shell command
typedef struct {
	char* arguments[20]; // Arguments value
	int length; // Arguments length
	char* input_file;
	char* output_file;
} Command;

Command* get_commands(char* line, int* numberOfCommands, int* error) {


	char *ptr = strdup(line);
	*numberOfCommands = 0;
	*error = 0;
	char* token;

	while ((token = strsep(&ptr, "&")) != NULL) {
		(*numberOfCommands)++;
	}

	free(ptr);
	ptr = line;
	Command* commands = (Command*) malloc(sizeof(Command) * (*numberOfCommands));
	int commandNumber = 0;
	int commandLength = 0;

	commands[0].input_file = NULL;
	commands[0].output_file = NULL;

	while ((token = strsep(&ptr, " ")) != NULL) {
		if (
			(commands[commandNumber].output_file != NULL ||
			commands[commandNumber].input_file != NULL) &&
			strcmp(token, "&") != 0
		) {
			*error = 1;
			return NULL;
		}

		if (strcmp(token, ">") == 0) {  // Redirección de salida
			commands[commandNumber].output_file = strsep(&ptr, " ");
			
			if (commands[commandNumber].output_file == NULL) {
				*error = 1;
				return NULL;
			}
		} else if (strcmp(token, "<") == 0) {  // Redirección de entrada
			commands[commandNumber].input_file = strsep(&ptr, " ");
			if (commands[commandNumber].input_file == NULL) {
				*error = 1;
				return NULL;
			}
		} else if (strcmp(token, "&") == 0) {
			if (commandLength == 0) {
				(*numberOfCommands)--;
				*error = 1;
				return NULL;
			}

			commands[commandNumber].arguments[commandLength] = NULL;
			commands[commandNumber].length = commandLength;
	
			commandNumber++;
			commands[commandNumber].input_file = NULL;
			commands[commandNumber].output_file = NULL;

			commandLength = 0;
		
		} else if (token[0] != '\0') {
			commands[commandNumber].arguments[commandLength++] = token;
		}
	}

	commands[commandNumber].arguments[commandLength] = NULL;	
	commands[commandNumber].length = commandLength;
	if (commandLength == 0 && *numberOfCommands > 1) {
		(*numberOfCommands)--;
	}

	return commands;
}

int all_spaces(const char* line) {
	for (int i = 0; i < strlen(line); i++) {
		if (!isspace(line[i])) {
			return 0;
		}
	}

	return 1;
}

int main(int argc, char* argv[]) {
	FILE* read_file = NULL;
	char* line = NULL;
	size_t size = 0;

	if (argc > 2) {
		fprintf(stderr, "An error has occurred\n");
		exit(1);
	}

	read_file = stdin;

	if (argc == 2) {
		read_file = fopen(argv[1], "r");

		if (read_file == NULL) {
			fprintf(stderr, "An error has occurred\n");
			exit(1);
		}
	}

	if (argc != 2)
		printf("UVash> ");
	
	int i = 0;
	int error = 0;
	Command* commands;
	int numberOfCommands;
	pid_t pid;
	pid_t* pids;

	while (getline(&line, &size, read_file) != EOF) {
		line[strcspn(line, "\n")] = '\0';

		if (all_spaces(line))
			continue;

		commands = get_commands(line, &numberOfCommands, &error);
		pids = (pid_t*) malloc(sizeof(pid_t) * numberOfCommands);

		for (i = 0; i < numberOfCommands; i++) {
			if (error || commands[i].length == 0) {
				fprintf(stderr, "An error has occurred\n");
				continue;
			}


			if (strcmp(commands[i].arguments[0], "exit") == 0) {
				if (commands[i].length != 1) {
					fprintf(stderr, "An error has occurred\n");
				} else {
					exit(0);
				}
			} else if (strcmp(commands[i].arguments[0], "cd") == 0) {
				if (commands[i].length != 2) {
					fprintf(stderr, "An error has occurred\n");
				} else if (chdir(commands[i].arguments[1]) != 0) {
					perror("chdir failed");
				}
			} else {
				pid = fork();
				
				if (pid < 0) {
					perror("fork");
					return 1;
				} else if (pid == 0) {
					if (commands[i].input_file) {
						int fd_in = open(commands[i].input_file, O_RDONLY);
						if (fd_in == -1) {
						    perror("Error abriendo archivo de entrada");
						    exit(1);
						}
						dup2(fd_in, STDIN_FILENO);
						close(fd_in);
					}

					if (commands[i].output_file) {
						int fd_out = open(commands[i].output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
						if (fd_out == -1) {
						    perror("Error abriendo archivo de salida");
						    exit(1);
						}
						dup2(fd_out, STDOUT_FILENO);
						close(fd_out);
					}
					
					execvp(commands[i].arguments[0], commands[i].arguments);
					fprintf(stderr, "An error has occurred\n");
					exit(1);
				} else {
					pids[i] = pid;
				}
			}
		}

		for (i = 0; i < numberOfCommands; i++) {
			waitpid(pids[i], NULL, 0);
		}

		free(pids);
		free(commands);
		free(line);
		size = 0;
		if (argc != 2)
			printf("UVash> ");
	}

	exit(0);
}
