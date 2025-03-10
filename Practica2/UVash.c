#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

int main(int argc, char* argv[]) {
	FILE* read_file = NULL;
        char* input_file = NULL;
	char* output_file = NULL;
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
			if (strcmp(token, ">") == 0) {  // Redirección de salida
                		output_file = strsep(&ptr, " ");
            		
			} else if (strcmp(token, "<") == 0) {  // Redirección de entrada
                		input_file = strsep(&ptr, " ");
			
			} else if (token[0] != '\0') {
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
				if (input_file) {
					int fd_in = open(input_file, O_RDONLY);
					if (fd_in == -1) {
					    perror("Error abriendo archivo de entrada");
					    exit(1);
					}
					dup2(fd_in, STDIN_FILENO);
					close(fd_in);
				}

				if (output_file) {
					int fd_out = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
					if (fd_out == -1) {
					    perror("Error abriendo archivo de salida");
					    exit(1);
                			}
                			dup2(fd_out, STDOUT_FILENO);
                			close(fd_out);
            			}
				
				execvp(args[0], args);
				fprintf(stderr, "An error has occurred\n");
				exit(1);
			} else {
				waitpid(pid, NULL, 0);
    			}
		}
		
		input_file = NULL;
		output_file = NULL;
		free(line);
		size = 0;
		i = 0;
		
		if (argc != 2)
			printf("UVash> ");
	}

	exit(0);
}
