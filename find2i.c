#include <stdio.h>
#include <stdlib.h>

#include "e2util.h"

int main(int argc, char *argv[])
{
	FILE *f;
	struct superblock sb;
	char *block;
	int i;

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

	block = malloc(blocksize(&sb));

	// Loop through all the blocks
	for (i = 1; i < sb.s_blocks_count; i++) {
		// We're looking for a free block...
		switch (is_block_free(&sb, i)) {
			case -1:
				printf("error in is_block_free %d\n", i);
				return 1;
			case 0:
				continue;
		}

		// Load in the block, assuming it's an indirect block...
		if (get_block_data(&sb, i, block))
			return 1;
		byteswap_iblock(&sb, block);

		// ... and check to see if it looks like a doubly-indirect
		// block.
		if (looks_2indirect(&sb, block))
			printf("%d\n", i);
	}

	free(block);
	fclose(f);
	return 0;
}
