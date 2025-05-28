#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "e2util.h"

int main(int argc, char *argv[])
{
	FILE *f;
	struct superblock sb;
	struct inode node;
	// Fill in the number of required arguments and usage here
/*	if (argc <= 100) {
		printf("Usage: %s <image file> ARGUMENTS\n", argv[0]);
		return 1;
	}*/

	f = fopen(argv[1], "r");
	if (f == NULL) {
		perror("fopen");
		return 1;
	}

	if (get_superblock(f, &sb))
		return 1;

	// Code here...
	int size = 1669338;
	node.i_size = size;
	int firstDirct = 4838;
	for( int i = 0; i < 12; i++){
		node.i_block_d[i] = firstDirct;
		firstDirct++;
	}
	node.i_block_1i = firstDirct;
	node.i_block_2i = 3522;
	print_inode_data(&sb,&node);

	fclose(f);
	return 0;
}
