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
 	
 	if (volume_scale < 1) {
 		fprintf(stderr, "Incorrect Volume Scale\n");
 	}
 	//Checking if the two files given are valid.
	if ((sourcewav = fopen(argv[5], "rb")) == NULL) {
		fprintf(stderr, "Can't open %s\n", argv[5]);
		exit(1);
	}

	if ((destwav = fopen(argv[6], "wb")) == NULL) {
		fprintf(stderr, "Can't open %s\n", argv[6]);
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
	echoBuffer = calloc(delay, sizeof(short));

	short *origBuffer;
	origBuffer = calloc(delay, sizeof(short));
	int i = 0;
	while (i < delay && fread(&origBuffer[i], sizeof(short), 1, sourcewav) == 1) {
		i++;
	}
	//writing the first delay samples mixed with 0 to the output file
	int j = 0;
	while (j < delay) {
		fwrite(&origBuffer[j], sizeof(short), 1, destwav);
		j++;
	} 

	int flag = 1;
	int samples = delay;
	int s = 0;
	short zero = 0;
	while (1) {
		//copying the contents of origBuffer to echoBuffer
		int k = 0;
		free(echoBuffer);
		echoBuffer = calloc(delay, sizeof(short));
		while (k < delay) {
			echoBuffer[k] = origBuffer[k];
			k++;
		}
		printf("%d\n", (short) sizeof(echoBuffer));
		printf("%d\n", (short) sizeof(origBuffer));
		//applying scaling by volume
		int l = 0;
		while (l < delay) {
			//echoBuffer[l] = echoBuffer[l] / (short) volume_scale;
			echoBuffer[l] = (short) echoBuffer[l] / (short) volume_scale;
			l++;
		}
		//copying next delay samples into origBuffer, but if there 
		//aren't samples left, fill the rest of the space with 0's.
		free(origBuffer);
		origBuffer = calloc(delay, sizeof(short));
		int n = 0;
		while (n < delay && flag != 0) {
			if (fread(&origBuffer[n], sizeof(short), 1, sourcewav) != 1) {
				int rest_of_file;
				s = n;
				for (rest_of_file = 0; rest_of_file < delay - n; rest_of_file++) {
					origBuffer[n] = zero;
				} 
				flag = 0;
				samples--;
			}
			samples++;
			n++;
		}

		//writing to output file
		int p = 0;
		while (p < delay) {
			short mixed = (short) echoBuffer[p] + (short) origBuffer[p];
			fwrite(&mixed, sizeof(short), 1, destwav);
			p++;
		}
		//checking to see if we've reached the end of our file
		if (flag == 0) {
			break;
		}
	}
	int q = 0;
	free(echoBuffer);
	echoBuffer = calloc(delay, sizeof(short));
	while (q < delay) {
		echoBuffer[q] = origBuffer[q];
		q++;
	}
	int u = 0;
	while (u < delay) {
		echoBuffer[u] = echoBuffer[u] / (short) volume_scale;
		u++;
	}
	if (origBuffer[0] != zero) {
	 	free(origBuffer);
	 	origBuffer = calloc(delay, sizeof(short));
		int r = 0;
		while (r < delay) {
			origBuffer[r] = zero;
	 		r++;
	    }
	}
	int t = 0;
	while (t < s) {
	    short mixed = (short) echoBuffer[t] + (short) origBuffer[t];
	    //short mixed = echoBuffer[t];
		fwrite(&mixed, sizeof(short), 1, destwav);
		t++;
	}

	fclose(sourcewav);
	fclose(destwav);
	return 0;
}//error checking for all system calls!!!!
//zeroes may also be messing up