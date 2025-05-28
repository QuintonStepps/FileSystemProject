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

	// Print the allocation status of the given block.
	switch (is_block_free(&sb, strtoul(argv[2], NULL, 0))) {
		case 0:
			printf("allocated\n");
			break;
		case 1:
			printf("free\n");
			break;
		default:
			printf("error\n");
			return 1;
	}

	fclose(f);
	return 0;
}
