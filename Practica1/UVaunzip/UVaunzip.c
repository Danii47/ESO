#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {

	if (argc < 2) {
		printf("UVaunzip: file1 [file2 ...]\n");
		exit(1);
	}

	int number_of_files = argc - 1;
	FILE** files = (FILE**) malloc(sizeof(FILE*) * number_of_files);

	if (files == NULL) {
		fprintf(stderr, "UVaunzip: cannot asign memory for files\n");
		exit(1);
	}

	int number;
	char character;

	for (int i = 0; i < number_of_files; i++) {
		files[i] = fopen(argv[i + 1], "r");

		if (files[i] == NULL) {
			fprintf(stderr, "UVaunzip: cannot open file\n");
			free(files);
			exit(1);
		}

		while (fread(&number, sizeof(int), 1, files[i]) != 0) {
			if (fread(&character, sizeof(char), 1, files[i]) == 0) {
				fprintf(stderr, "UVaunzip: bad compressed file\n");
				fclose(files[i]);
				free(files);
				exit(1);
			}

			for (int j = 0; j < number; j++) {
				printf("%c", character);
			}
		}

		fclose(files[i]);
	}

	free(files);

	exit(0);
}

			
