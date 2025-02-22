#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NOT_FILE_ARGUMENTS 1

int main(int argc, char* argv[]) {

	if (argc < 2) {
		printf("UVazip: file1 [file2 ...]\n");
		exit(1);
	}

	int numberOfFiles = argc - NOT_FILE_ARGUMENTS;
	FILE** files = (FILE**) malloc(sizeof(FILE*) * numberOfFiles);

	if (files == NULL) {
		fprintf(stderr, "Ocurrio un error al intentar asignar la memoria para los archivos\n");
		exit(1);
	}

	char character, last_character = -1;
	int occurrence;

	for (int i = 0; i < numberOfFiles; i++) {
		files[i] = fopen(argv[i + 1], "r");

		if (files[i] == NULL) {
			printf("UVazip: cannot open file\n");
			exit(1);
		}

		while ((character = fgetc(files[i])) != EOF) {
			if (last_character == character) {
				occurrence++;
			} else {
				if (last_character != -1) {
					fwrite(&occurrence, sizeof(int), 1, stdout);
					printf("%c", last_character);
				}

				occurrence = 1;
				last_character = character;
			}
		}

		fclose(files[i]);
	}

	fwrite(&occurrence, sizeof(int), 1, stdout);
	printf("%c", last_character);

	free(files);

	exit(0);
}

