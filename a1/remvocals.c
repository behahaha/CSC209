#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
	FILE *sourcewav, *destwav;

	if (argc != 3) {
		fprintf(stderr, "insufficient arguments\n");
		exit(EXIT_FAILURE);
	}

	if ((sourcewav = fopen(argv[1], "rb")) == NULL) {
		fprintf(stderr, "Can't open %s\n", argv[1]);
		exit(EXIT_FAILURE);
	}

	if ((destwav = fopen(argv[2], "wb")) == NULL) {
		fprintf(stderr, "Can't open %s\n", argv[2]);
		fclose(sourcewav);
		exit(EXIT_FAILURE);
	}

	//reading and coping the first 44 bytes
	unsigned char buffer[44];
	if (fread(&buffer, 1, 44, sourcewav) != 44) {
		fprintf(stderr, "fread error while reading initial 44 bytes\n");
	}
	/////DOUBLE CHECK THE BELOW!!!!!/////
	if (fwrite(&buffer, 1, 44, destwav) != 44) {
		fprintf(stderr, "fwrite error while writing initial 44 bytes.\n");
	}

	//applying the algorithm to the rest of the file
	while (1) {
		short left;
		if (fread(&left, sizeof(short), 1, sourcewav) != 1) {
			break;
		}
		short right;
		if (fread(&right, sizeof(short), 1, sourcewav) != 1) {
			break;
		}
		short combined = (left - right) / 2;
		fwrite(&combined, sizeof(combined), 1, destwav);
		fwrite(&combined, sizeof(combined), 1, destwav);
	}

	fclose(sourcewav);
	fclose(destwav);
	return 0;
}