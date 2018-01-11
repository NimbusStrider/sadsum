
/***********************************************************************
 * 
 *   Simple Action Digest (SAD)
 *   Version 0.1, Alpha Testing
 *
 *   Program works but needs more functionality,
 *   such as argument usage and error handling.
 * 
 * *********************************************************************
 */


#define _FILE_OFFSET_BITS  64
	// Allows files larger than 2GB to be read.

#include <stdio.h>
#include <stdlib.h>

#include "piTable.h"
#define TABLE_NAME piTable
#define TABLE_SIZE 65536

#define BLOCK_SIZE 8192
	// Has no effect on the result.

#define MAX_DIGEST_BITS 16384
#define DEFAULT_DIGEST_BITS 512
	// Default will be rounded up to nearest value divisible by 6.


int main(int argc, char *argv[])
{
	const char base64[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	const int sizeDigest = (DEFAULT_DIGEST_BITS + 5)/6;
	const int sizeBytes = ((sizeDigest*6)+7)/8;

	unsigned char table[TABLE_SIZE];
	unsigned char data[BLOCK_SIZE];
	unsigned char digest[sizeDigest + 1];
	unsigned char digestBytes[sizeBytes + 1];
	
	FILE *targetfile;
	char *targetname;
	int len, n;

	int lenDigest, lenBytes;
	int i, j, k;
	unsigned short int x, y;
	

	// Check that the compiler is using 16-bit values for short int
	
	if (sizeof(short int) != 2) {
		printf("\nInternal Error: Data type \"short int\" is a %i-bit value. ", sizeof(short int)*8);
		printf("Needs to be a 16-bit value.\n\n");
		return 0;
	}
	
	if ((sizeDigest*6) > MAX_DIGEST_BITS) {
		printf("\nError: Maximum digest allowed is %i bits\n\n", MAX_DIGEST_BITS);
	}
	
	// Temporary input stuff, until I get around to incorporating <argp.h>
	
	if (argc != 2) {
			int zz = 0;
			if (argv[0][1] == '/') zz = 2;			
			printf("\nUsage: %s <filename>\n\n", &argv[0][zz]);
			return 1;
	}
	targetname = argv[1];
	
	
	
	// Open target file


	targetfile = fopen(targetname, "rb");
	
	if (!targetfile) {
		perror("Unable to open target file");
        return EXIT_FAILURE;
	}


	// Initialize

	for (i = 0; i < TABLE_SIZE; i++) {
		table[i] = TABLE_NAME[i];
	}
	x = 0;
	y = 0;
	n = 0;
	

	// Process target file

	len = fread(&data, 1, 1, targetfile);
	while (len > 0) {

		for (n = 0; n < len; n++) {
			x--;
			y += data[n] + 1;
			table[x] += table[y];
		}
		
		len = fread(&data, 1, BLOCK_SIZE, targetfile);

	}

	fclose(targetfile);



	// Generate digest
	
	for (lenBytes = 0; lenBytes < sizeBytes; lenBytes++) {

		for (i = 0; i < TABLE_SIZE; i++) {
		
			y += (table[i] *256) + table[x];
			table[i] += table[y];
	
		}		
		
		digestBytes[lenBytes] = table[y];

	}


	// Convert to Base-64

	k = 0;
	i = 0;
	for (lenDigest = 0; lenDigest < sizeDigest; lenDigest++) {
 		switch (k) {
		
			case 0:
				j = digestBytes[i] / 4;
				digest[lenDigest] = base64[j];
				k = 1;
				break;
			
			case 1:
				j = digestBytes[i] / 4;
				j *= 4;
				j = digestBytes[i] - j;
				i++;
				j += digestBytes[i] / 16;				
				digest[lenDigest] = base64[j];
				k = 2;
				break;
			
			case 2:
				j = digestBytes[i] / 16;
				j *= 16;
				j = digestBytes[i] - j;
				j *= 4;
				i++;
				j += digestBytes[i] / 64;
				digest[lenDigest] = base64[j];
				k = 3;
				break;
			
			case 3:
				j = digestBytes[i] / 64;
				j *= 64;
				j = digestBytes[i] - j;
				i++;
				digest[lenDigest] = base64[j];
				k = 0;
				break;			
		}		
	}
	digest[lenDigest] = 0;

	printf("%s  %s\n", digest, targetname);
	
	return EXIT_SUCCESS;
}

