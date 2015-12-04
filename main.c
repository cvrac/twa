#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tiny_warehouse.h"

#define INITBUCKS 2

int main(int argc, char **argv) {

	char *datafile = NULL, *operationsfile = NULL, *configfile = NULL;
	record rec;
	listptr totable;
	int bucketentries = 2, numOfrecords, i, compargs = 0, lplace = 0, pplace = 0, cplace = 0;
	long lSize;
	float loadfactor = 0.75;
	twaptr twa;
	long long cid;
	if (argc < 5) {
		printf("Sorry, not enough arguments. Program needs at least 5 arguments to execute\n");
		return 1;
	}

	for (i = 1; i < argc; i = i+2)
		if (strcmp(argv[i], "-b") == 0 || strcmp(argv[i], "-f") == 0)
			compargs++;

	if (compargs < 2) {
		printf("Arguments -b, for Bucket entries, and -f for Load actor are needed for program execution\n");
		return 1;
	}

	for (i = 1; i < argc; i = i+2) {
		if (strcmp(argv[i], "-b") == 0) {
			bucketentries == atoi(argv[i+1]);
		} else if (strcmp(argv[i], "-f") == 0) {
			loadfactor = atof(argv[i+1]);
		} else if (strcmp(argv[i], "-l") == 0) {
			datafile = malloc(strlen(argv[i+1]) + 1);
			datafile = strcpy(datafile, argv[i+1]);
		} else if (strcmp(argv[i], "-p") == 0) {
                        operationsfile = malloc(strlen(argv[i+1]) + 1);
                        operationsfile = strcpy(operationsfile, argv[i+1]);
		} else if (strcmp(argv[i], "-c") == 0) {
                        configfile = malloc(strlen(argv[i+1]) + 1);
                        configfile = strcpy(configfile, argv[i+1]);
		}
	}
	twa = twa_constructor(INITBUCKS, bucketentries, loadfactor, datafile, operationsfile, configfile);

	twa_prompt(twa);

	return 0;
}
			
