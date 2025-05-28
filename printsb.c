#include <stdio.h>

#include "e2util.h"

int main(int argc, char *argv[])
{
	FILE *f;
	struct superblock sb;

	if (argc < 2) {
		printf("Usage: %s <image file>\n", argv[0]);
		return 1;
	}

	f = fopen(argv[1], "r");
	if (f == NULL) {
		perror("fopen");
		return 1;
	}

	if (get_superblock(f, &sb))
		return 1;

	// Print out the data in the superblock
	print_superblock(&sb);

	fclose(f);
	return 0;
}
