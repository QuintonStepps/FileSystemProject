#include <stdio.h>
#include <stdlib.h>

#include "e2util.h"

int main(int argc, char *argv[])
{
	FILE *f;
	struct superblock sb;
	struct inode inode;

	if (argc < 3) {
		printf("Usage: %s <image file> <inode #>\n", argv[0]);
		return 1;
	}

	f = fopen(argv[1], "r");
	if (f == NULL) {
		perror("fopen");
		return 1;
	}

	if (get_superblock(f, &sb))
		return 1;

	// Fetch the specified inode
	if (get_inode(&sb, strtoul(argv[2], NULL, 0), &inode))
		return 1;

	// Print out the data contained in the inode
	if (print_inode_data(&sb, &inode))
		return 1;

	fclose(f);
	return 0;
}
