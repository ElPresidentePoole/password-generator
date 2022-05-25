#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/random.h>

int main(int argc, char* argv[]) {
	bool omitLetters = false;
	bool omitNumbers = false;
	bool omitSymbols = false;

	char empty[] = "";
	char letters[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
	char numbers[] = "123457890";
	char symbols[] = "!@#$%^&*();:{}[]\\|<>,./?`~";

	int opt;
	int pw_length = 25;
	while((opt = getopt(argc, argv, "lnsL:")) != -1) {
		switch(opt) {
			case 'l': omitLetters = true; break;
			case 'n': omitNumbers = true; break;
			case 's': omitSymbols = true; break;
			case 'L': pw_length = atoi(optarg); if(!pw_length) { fprintf(stderr, "%s: -L option needs valid integer", argv[0]); exit(EXIT_FAILURE); } break;
			default: fprintf(stderr, "Usage: %s [-lnsL (n)]\n", argv[0]); exit(EXIT_FAILURE);
		}
	}

	if(omitLetters && omitNumbers && omitSymbols) {
		fprintf(stderr, "%s requires at least one allowed group\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	char all[256];
	snprintf(all, sizeof(all), "%s%s%s", 
			omitLetters ? empty : letters, 
			omitNumbers ? empty : numbers, 
			omitSymbols ? empty : symbols);

	/* alright now it's time to get funky
	   so if we use srand and seed it with our current time, each second will
	   generate the same password.  so if we run this program 50 times a second,
	   all 50 passwords will be the same.  the solution?  read N bytes from
	   /dev/urandom and write it directly to where our seed is in memory.
	*/
	FILE* urand = fopen("/dev/urandom", "rb");
	if(urand == NULL) {
		fprintf(stderr, "%s:failed to open /dev/urandom\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	unsigned long seed;
	fread(&seed, sizeof(unsigned long), 1, urand);
	srand(seed);
	fclose(urand);

	char pw[256];
	for(int i = 0; i < pw_length; i++) {
		pw[i] = all[rand() % strlen(all)];
	}
	pw[pw_length] = '\0';
	printf("%s\n", pw);

	return EXIT_SUCCESS;
}
