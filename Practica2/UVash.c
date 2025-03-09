#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char* argv[]) {
	FILE* read_file = NULL;
	char* line = NULL;
	size_t size = 0;

	if (argc > 2) {
		exit(1);
	}

	read_file = stdin;

	if (argc == 2) {
		read_file = fopen(argv[1], "r");

		if (read_file == NULL) {
			exit(1);
		}
	}

	if (argc != 2)
		printf("UVash> ");
	
	char* token;
	char *args[20];
	int i = 0;
	pid_t pid;
	while (getline(&line, &size, read_file) != EOF) {
		line[strcspn(line, "\n")] = '\0';
		char *ptr = line;
		while ((token = strsep(&ptr, " ")) != NULL) {
			if (*token != '\0') {
				args[i++] = token;
			}
		}
		args[i] = NULL;

		if (strcmp(args[0], "exit") == 0) {
			if (i != 1) {
				fprintf(stderr, "An error has occurred\n");
			} else {
				exit(0);
			}
		} else if (strcmp(args[0], "cd") == 0) {
			if (i != 2) {
				fprintf(stderr, "An error has occurred\n");
			} else {
				chdir(args[1]);
			}
		} else {
			pid = fork();
			
			if (pid < 0) {
				perror("fork");
				return 1;
			} else if (pid == 0) {
				execvp(args[0], args);
				fprintf(stderr, "An error has occurred\n");
				exit(1);
			} else {
				waitpid(pid, NULL, 0);
    			}
		}

		free(line);
		size = 0;
		i = 0;
		
		if (argc != 2)
			printf("UVash> ");
	}

	exit(0);
}
