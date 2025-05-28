#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "e2util.h"

int main(int argc, char *argv[])
{
	FILE *f;
	struct superblock sb;
	int i, len;
	char *block;

	if (argc < 3) {
		printf("Usage: %s <image file> <search string>\n", argv[0]);
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
	if (block == NULL)
		return 1;

	// Search free blocks
	len = strlen(argv[2]);
	for (i = 0; i < sb.s_blocks_count; i++) {
		if (!is_block_free(&sb, i))
			continue;
		if (get_block_data(&sb, i, block))
			return 1;
		if (!strncmp(argv[2], block, len))
			printf("%d\n", i);
	}

	free(block);
	fclose(f);
	return 0;
}
