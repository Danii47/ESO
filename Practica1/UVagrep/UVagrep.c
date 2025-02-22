#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_SIZE 5000
#define NOT_FILE_ARGUMENTS 2

void search_in_file(const char *search_term, FILE* file) {
	char* buffer = (char*) malloc(sizeof(char*) * MAX_LINE_SIZE);

	if (buffer == NULL) {
		fprintf(stderr, "No fue posible asignar la memoria para el buffer\n");
		exit(1);
	}

	while (fgets(buffer, MAX_LINE_SIZE, file) != NULL) {
		if (strstr(buffer, search_term) != NULL) {
			printf("%s", buffer);
		}
	}
}

	
int main(int argc, char* argv[]) {

	if (argc < 2) {
		printf("UVagrep: searchterm [file ...]\n");
		exit(1);
	}

	char* search = argv[1];
	int numberOfFiles = argc - NOT_FILE_ARGUMENTS;

	if (numberOfFiles == 0) {
		search_in_file(search, stdin);
	} else {

		FILE** files = (FILE**) malloc(sizeof(FILE*) * numberOfFiles);

		if (files == NULL) {
			fprintf(stderr, "No fue posible asignar la memoria para los ficheros\n");
			exit(1);
		}

		for (int i = 0; i < numberOfFiles; i++) {
			files[i] = fopen(argv[i + 2], "r");

			if (files[i] == NULL) {
				printf("UVagrep: cannot open file\n");
				exit(1);
			}


			search_in_file(search, files[i]);
			fclose(files[i]);
		}
	}

	exit(0);
}

