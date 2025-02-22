#include <stdio.h>
#include <stdlib.h>

#define MAX_LINE_SIZE 5000
#define NOT_FILE_ARGUMENTS 1

int main(int argc, char* argv[]) {

	if (argc < 2) {
		exit(0);
	}

	int numberOfFiles = argc - NOT_FILE_ARGUMENTS;
	FILE** files = (FILE**) malloc(sizeof(FILE*) * numberOfFiles);
	
	if (files == NULL) {
		fprintf(stderr, "UVacat: Ocurrio un error al intentar asignar la memoria\n");
		exit(1);
	}

	char* buffer = (char*) malloc(sizeof(char) * MAX_LINE_SIZE);

	if (buffer == NULL) {
		fprintf(stderr, "UVacat: Ocurrio un error al intentar asignar la memoria\n");
	}

	for (int i = 0; i < numberOfFiles; i++) {
		files[i] = fopen(argv[i + 1], "r");

		if (files[i] == NULL) {
			// Yo hubiera puesto fprintf(stderr, "..."); pero no lo permiten los tests
			printf("UVacat: no puedo abrir fichero\n");
			exit(1);
		}

		while (fgets(buffer, MAX_LINE_SIZE, files[i]) != NULL) {
			printf("%s", buffer);
		}

		fclose(files[i]);
	}

	free(files);
	free(buffer);

	exit(0);
}

