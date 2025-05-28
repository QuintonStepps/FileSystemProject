#include <stdio.h>
#include <stdlib.h>

#include "e2util.h"

int main(int argc, char *argv[])
{
	FILE *f;
	struct superblock sb;

	if (argc < 3) {
		printf("Usage: %s <image file> <block #>\n", argv[0]);
		return 1;
	}

	f = fopen(argv[1], "r");
	if (f == NULL) {
		perror("fopen");
		return 1;
	}

	if (get_superblock(f, &sb))
		return 1;

	// Print out the data in this block
	if (print_block_data(&sb, strtoul(argv[2], NULL, 0)))
		return 1;

	fclose(f);
	return 0;
}
