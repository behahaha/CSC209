#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#define HEADER 22
#define AUTO_DELAY 8000
#define AUTO_VOLUME 4

int main(int argc, char *argv[]) {
	FILE *sourcewav = NULL;
	FILE *destwav = NULL;
	int delay = AUTO_DELAY;
	int volume_scale = AUTO_VOLUME;
	int count = 0;
	int c;
	opterr = 0;

	if (argc < 3) {
		fprintf(stderr, "insufficient arguments\n");
		exit(1);
	}

	//processing command line arguments.
	while ((c = getopt(argc, argv, "d:v:")) != -1 && count != 2) {
		switch (c) {
			case 'd':
				delay = strtol(optarg, NULL, 10);
				break;
			case 'v':
				volume_scale = strtol(optarg, NULL, 10);
				break;
			case '?':
			default:
				if (optopt == 'd') {
					fprintf(stderr, "Option -%c requires an argument.\n", optopt);
				} else if (optopt == 'v') {
					fprintf(stderr, "Option -%c requires an argument.\n", optopt);
				} else {
					fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
				}
		} count++;
	}
 	
 	//Checking if the two files given are valid.
	if ((sourcewav = fopen(argv[5], "rb")) == NULL) {
		fprintf(stderr, "1Can't open %s\n", argv[5]);
		exit(1);
	}

	if ((destwav = fopen(argv[6], "wb")) == NULL) {
		fprintf(stderr, "2Can't open %s\n", argv[6]);
		fclose(sourcewav);
		exit(1);
	}

	//first read the first 44 bytes (22 short, 11 int, HAHAH) and change the header!
	short header[HEADER];
	if ((fread(&header, sizeof(short), HEADER, sourcewav)) != HEADER) {
		perror("fread");
		exit(1);
	}
	unsigned int *sizeptr = (unsigned int *)(header + 2);
	unsigned int adder = delay * 2;
	*sizeptr = *sizeptr + adder;
	unsigned int *sizeptr2 = (unsigned int *)(header + 20);
	*sizeptr2 = *sizeptr2 + adder;
	
	fwrite(&header, sizeof(short), 22, destwav);

	//reading samples from the original wav file
	short *echoBuffer;
	echoBuffer = malloc(delay);
	int i = 0;

	short *origBuffer;
	origBuffer = malloc(delay);
	i = 0;
	while (i < delay && fread(&origBuffer[i], sizeof(short), 1, sourcewav) == 1) {
		i++;
	}
	//writing the first delay samples mixed with 0 to the output file
	i = 0;
	while (i < delay) {
		fwrite(&origBuffer[i], sizeof(short), 1, destwav);
		i++;
	} 
	// printf("%s\n", "FIRST ORIG");
	// int m = 0;
	// while (m < delay) {
	// 	printf("%d\n", origBuffer[m]);
	// 	m++;
	// }

	int flag = 1;
	short zero = 0;
	while (1) {
		//copying the contents of origBuffer to echoBuffer
		i = 0;
		free(echoBuffer);
		echoBuffer = malloc(delay);
		while (i < delay) {
			echoBuffer[i] = origBuffer[i];
			i++;
		}
		//applying scaling by volume
		i = 0;
		while (i < delay) {
			echoBuffer[i] = echoBuffer[i] / (short) volume_scale;
			i++;
		}
		//copying next delay samples into origBuffer, but if there 
		//aren't samples left, fill the rest of the space with 0's.
		i = 0;
		free(origBuffer);
		origBuffer = malloc(delay);
		if (flag == 2) {
			while (i < delay) {
				origBuffer[i] = zero;
				i++;
			}
			i = 0;
			flag = 0;
		}
		
		while (i < delay && flag != 0) {
			if (fread(&origBuffer[i], sizeof(short), 1, sourcewav) != 1) {
				int rest_of_file;
				for (rest_of_file = 0; rest_of_file < delay - i; rest_of_file++) {
					origBuffer[i] = zero;
				}
				i = delay;
				flag = 0;
			} 
			i++;
		}

		//writing to output file
		i = 0;
		while (i < delay && echoBuffer[0] != zero) {
			short mixed = echoBuffer[i] + origBuffer[i];
			fwrite(&mixed, sizeof(short), 1, destwav);
			i++;
		}
		if (flag == 0 && origBuffer[0] != zero) {
			flag = 2;
		}
		//checking to see if we've reached the end of our file
		if (flag == 0 && origBuffer[0] == zero) {
			break;
		}
	}
	fclose(sourcewav);
	fclose(destwav);
	return 0;
}//error checking for all system calls!!!!
//zeroes may also be messing up

// do last mixing after the while loop?