#include <stdio.h>
#include <stdlib.h>

#include "e2util.h"

int main(int argc, char *argv[])
{
	FILE *f;
	struct superblock sb;
	struct bgdesc bg;

	if (argc < 3) {
		printf("Usage: %s <image file> <block group #>\n", argv[0]);
		return 1;
	}

	f = fopen(argv[1], "r");
	if (f == NULL) {
		perror("fopen");
		return 1;
	}

	if (get_superblock(f, &sb))
		return 1;

	// Fetch the block group descriptor
	if (get_bgdesc(&sb, strtoul(argv[2], NULL, 0), &bg))
		return 1;

	// Print out the components
	printf("Block bitmap = 0x%x, inode bitmap = 0x%x, inode table = 0x%x\n",
			bg.bg_block_bitmap,
			bg.bg_inode_bitmap,
			bg.bg_inode_table);

	fclose(f);
	return 0;
}
