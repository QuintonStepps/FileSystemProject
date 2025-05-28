#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <time.h>

#include "e2util.h"


// Switch all of the values in the superblock structure from ext2 little-endian
// to the host's byte order.
void byteswap_superblock(struct superblock *sb)
{
	SWAP_LE(sb->s_inodes_count, 32);
	SWAP_LE(sb->s_blocks_count, 32);
	SWAP_LE(sb->s_first_data_block, 32);
	SWAP_LE(sb->s_log_block_size, 32);
	SWAP_LE(sb->s_blocks_per_group, 32);
	SWAP_LE(sb->s_inodes_per_group, 32);
	SWAP_LE(sb->s_state, 16);
}

// Display a formatted output of the superblock parameters.
void print_superblock(struct superblock *sb)
{
	printf("Inodes: %u\n"
	       "Blocks: %u\n"
	       "First data block: %u\n"
	       "Block size: %u\n"
	       "Blocks/group: %u\n"
	       "Inodes/group: %u\n"
	       "State: %s\n",
			sb->s_inodes_count, sb->s_blocks_count,
			sb->s_first_data_block, blocksize(sb),
			sb->s_blocks_per_group, sb->s_inodes_per_group,
			sb->s_state == 1 ? "Clean" : "Dirty");
}

// Switch all of the values in the inode structure from ext2 little-endian to
// the host's byte order.
void byteswap_bgdesc(struct bgdesc *bg)
{
	SWAP_LE(bg->bg_block_bitmap, 32);
	SWAP_LE(bg->bg_inode_bitmap, 32);
	SWAP_LE(bg->bg_inode_table, 32);
}

// Switch all of the values in the inode structure from ext2 little-endian to
// the host's byte order.
void byteswap_inode(struct inode *i)
{
	int j;

	SWAP_LE(i->i_mode, 16);
	SWAP_LE(i->i_uid, 16);
	SWAP_LE(i->i_size, 32);
	SWAP_LE(i->i_atime, 32);
	SWAP_LE(i->i_ctime, 32);
	SWAP_LE(i->i_mtime, 32);
	SWAP_LE(i->i_dtime, 32);
	for (j = 0; j < 12; j++)
		SWAP_LE(i->i_block_d[j], 32);
	SWAP_LE(i->i_block_1i, 32);
	SWAP_LE(i->i_block_2i, 32);
	SWAP_LE(i->i_block_3i, 32);
}

// Display a formatted output of the inode parameters.
void print_inode(struct inode *i)
{
	time_t t;

	printf("Mode: %o\n"
	       "User ID: %u\n"
	       "Size: %u\n",
			i->i_mode, i->i_uid, i->i_size);
	t = i->i_atime;
	printf("Access time: %s", ctime(&t));
	t = i->i_ctime;
	printf("Change time: %s", ctime(&t));
	t = i->i_mtime;
	printf("Modification time: %s", ctime(&t));
	t = i->i_dtime;
	printf("Deletion time: %s", ctime(&t));
	printf("First direct block: %u\n", i->i_block_d[0]);
}

// Print out all the data in the file represented by a certain inode.
// Return 0 on success, 1 on error.
int print_inode_data(struct superblock *sb, struct inode *i)
{
	int fullblocks = i->i_size / blocksize(sb);
	int j;
	char *block;

	block = malloc(blocksize(sb));
	if (block == NULL)
		return 1;

	for (j = 0; j < fullblocks; j++) {
		if (get_inode_block(sb, i, j, block))
			return 1;
		if (fwrite(block, blocksize(sb), 1, stdout) != 1)
			return 1;
	}
	if (i->i_size % blocksize(sb)) {
		if (get_inode_block(sb, i, j, block))
			return 1;
		if (fwrite(block, i->i_size % blocksize(sb), 1, stdout) != 1)
			return 1;
	}

	free(block);
	return 0;
}

// Switch all of the values in an indirect block from ext2 little-endian to the
// host's byte order.
void byteswap_iblock(struct superblock *sb, char *block)
{
	int i;
	uint32_t *entry = (uint32_t *) block;
	for (i = 0; i < blocksize(sb) / 4; i++)
		SWAP_LE(entry[i], 32);
}

// Returns the block size of the filesystem
int blocksize(struct superblock *sb)
{
	return 1024 << sb->s_log_block_size;
}

// --- end provided code --- //


// Retrieve the interesting parts of the superblock and store it in the struct.
// Return 0 on success, 1 on error.
int get_superblock(FILE *f, struct superblock *out)
{
	// Save the file so other functions can use it
	out->file = f;
	
	int bumber = fseek(f, 1024,SEEK_SET);
	if(bumber < 0){
		perror("Bad Seek");
	}
	int array[1];
	bumber = fread(array,4,1,f);
	//Number of Inodes
	out->s_inodes_count = array[0];

	bumber = fread(array,4,1,f);
	if(bumber < 0){
		perror("Bad Seek");
	}
	out->s_blocks_count = array[0];
	if(fread(array,4,1,f) < 1){
		perror("Bad seek");
	}
	if(fread(array,4,1,f) < 1){
		perror("Bad seek");
	}
	if(fread(array,4,1,f) < 1){
		perror("Bad seek");
	}

	//gets the first block
	if(fread(array,4,1,f) < 1){
		perror("Bad seek");
	}
	out->s_first_data_block = array[0];

	//gets block size
	if(fread(array,4,1,f) < 1){
		perror("Bad seek");
	}
	out->s_log_block_size = array[0];
	
	if(fread(array,4,1,f) < 1){
		perror("Bad seek");
	}
	//gets number of blocks in each block group
	if(fread(array,4,1,f) < 1){
		perror("Bad seek");
	}
	out->s_blocks_per_group = array[0];

	if(fread(array,4,1,f) < 1){
		perror("Bad seek");
	}

	//gets Inodes per group
	if(fread(array,4,1,f) < 1){
		perror("Bad seek");
	}
	out->s_inodes_per_group = array[0];
	//print_superblock(out);
	// Code here...

	// Convert the superblock from little-endian format to whatever the
	// host system is.  Leave this at the end of get_superblock.
	byteswap_superblock(out);

	return 0;
}

// Fetch the data from the specified block into the provided buffer.
// Return 0 on success, 1 on error.
int get_block_data(struct superblock *sb, int blk, char *out)
{
	FILE *f =sb->file;
	//fseek(f,1024,SEEK_SET);
	fseek(f, blocksize(sb)*blk,SEEK_SET);
	if(fread(out,1024,1,f)< 1){
		perror("Bad seek in get block data\n");
		return 1;
	}

	return 0;
}

// Write the data from the specified block to standard output.
// Return 0 on success, 1 on error.
int print_block_data(struct superblock *sb, int blk)
{
	char out[1024];
	get_block_data(sb,blk,out);
	fwrite(out,1,1024,stdout);
	
	return 0;
}

// Return the number of the block group that a certain block belongs to.
int bg_from_blk(struct superblock *sb, int blk)
{
	return (blk - sb->s_first_data_block )/sb->s_blocks_per_group;
	
}

// Return the index of a block within its block group.
int blk_within_bg(struct superblock *sb, int blk)
{
	return (blk - sb->s_first_data_block )- (bg_from_blk(sb,blk) *sb->s_blocks_per_group);
}

// Return the number of the block group that a certain inode belongs to.
int bg_from_ino(struct superblock *sb, int ino)
{
	return ino/ sb->s_inodes_per_group;
}

// Return the index of an inode within its block group
int ino_within_bg(struct superblock *sb, int ino)
{
	return (ino - 1) % sb->s_inodes_per_group;
}

// Retrieve information from the block group descriptor table.
// Return 0 on success, 1 on error.
int get_bgdesc(struct superblock *sb, int bg, struct bgdesc *out)
{
	
	fseek(sb->file, (blocksize(sb) + 1024) +32 * bg, SEEK_SET);
	fread(&out->bg_block_bitmap,4,1,sb->file);
	
	fread(&out->bg_inode_bitmap,4,1,sb->file);
	fread(&out->bg_inode_table,4,1,sb->file);
	// Code here...

	// Convert the block info from little-endian format to whatever the
	// host system is.  Leave this at the end of get_bgdesc.
	byteswap_bgdesc(out);

	return 0;
}

// Retrieve information from an inode (file control block).
// Return 0 on success, 1 on error.
int get_inode(struct superblock *sb, int ino, struct inode *out)
{
	// Code here...
	int bingo[1];
	int block = bg_from_ino(sb,ino);
	int index = ino_within_bg(sb, ino);
	struct bgdesc bingus;
	get_bgdesc(sb, block, &bingus);
	fseek(sb->file, blocksize(sb)* bingus.bg_inode_table + (index*128), SEEK_SET);
	
	fread(&out->i_mode,2,1,sb->file);
	fread(&out->i_uid,2,1,sb->file);
	fread(&out->i_size,4,1,sb->file);
	fread(&out->i_atime,4,1,sb->file);
	fread(&out->i_ctime,4,1,sb->file);
	fread(&out->i_mtime,4,1,sb->file);
	fread(&out->i_dtime,4,1,sb->file);
	//1
	/*fread(out->i_block_d,4,1,sb->file);
	//2
	fread(out->i_block_d,4,1,sb->file);
	//3
	fread(out->i_block_d,4,1,sb->file);
	//4
	fread(out->i_block_d,4,1,sb->file);
	*/
	fseek(sb->file, blocksize(sb)* bingus.bg_inode_table + (index*128) + 40, SEEK_SET);

	fread(out->i_block_d,4,12,sb->file);
	fread(&out->i_block_1i,4,1,sb->file);
	fread(&out->i_block_2i,4,1,sb->file);
	fread(&out->i_block_3i,4,1,sb->file);
	

	// Convert the inode from little-endian format to whatever the host
	// system is.  Leave this at the end of get_inode.
	byteswap_inode(out);
	
	return 0;

}

// Retrieves the data from the nth data block of a certain inode.
// Return 0 on success, 1 on error.
int get_inode_block(struct superblock *sb, struct inode *i, uint32_t n, char *out)
{
	int size = blocksize(sb);
	int ppb = size / 4;

	if( n < 12){
	get_block_data(sb,i->i_block_d[n],out);
	return 0;
}

char * doubleindirect = malloc(size);
char * indirect = malloc(size);
n -= 12; //decrement to move on to indirect
	if (n < ppb) { // singly indirect blocks
	
	  get_block_data(sb, i->i_block_1i, indirect);
	
	  byteswap_iblock(sb, indirect);
  
	  uint32_t *indirect_entries = (uint32_t *)indirect;
	  uint32_t block_num = indirect_entries[n];
  
	  int ret = get_block_data(sb, block_num, out);
	  free(indirect);
	  return 0;
	}
	 
	n = n - (blocksize(sb)/4);
	
	get_block_data(sb,i->i_block_2i,doubleindirect);
	byteswap_iblock(sb,doubleindirect);
	int32_t *fl = (uint32_t)doubleindirect;
	
	get_block_data(sb, fl[n/ppb],indirect);
	byteswap_iblock(sb,indirect);
	int32_t *bonk = (uint32_t)indirect;
	get_block_data(sb,bonk[n%ppb],out);

	return 0;





}

// Return 1 if a block is free, 0 if it is not, and -1 on error
int is_block_free(struct superblock *sb, int blk)
{
struct bgdesc ds;
  int group = bg_from_blk(sb, blk);
  int index = blk_within_bg(sb, blk);

  get_bgdesc(sb, group, &ds);

  //printf("Banana");
  int block = blocksize(sb);
  char *bitmap = malloc(block);
  if (bitmap == NULL) {
	return -1;
  }
  if (get_block_data(sb, ds.bg_block_bitmap, bitmap) != 0) {
    return -1;
  }
	int bite = index / 8;
	int bit = index % 8;
  
  	int right = (1 << bit);
	int left = (bitmap[bite]);
	
  
	
	return !(right & left);
}

// Return 1 if a block appears to be an indirect block, 0 if it does not, and
// -1 on error.

	//int block = blocksize(sb);
int looks_indirect(struct superblock *sb, char *block)
{

	int size = blocksize(sb);
	int total = size / sizeof(uint32_t);
	uint32_t *list = (uint32_t *)block;
	int count = 0;
  
	if (list[0] == 0){
		return 0;
	}
	for (int i = 0; i < total; i++) { 
	  uint32_t address = list[i];
	  //invalid
	  if (address >= sb->s_blocks_count){
		return 0;
	  } 
	  count++;
	}
	if (count > 0){
		return 1;
	}
	else{
		return 0;
	}
  
}

// Return 1 if a block appears to be a doubly-indirect block, 0 if it does not,
// and -1 on error.
int looks_2indirect(struct superblock *sb, char *block)
{
int size = blocksize(sb);
  int total = size / sizeof(uint32_t);
  uint32_t *count = (uint32_t *)block;
    
  char *buffer = malloc(size);
  if (buffer == NULL) {
    perror("malloc");
    return -1;
  }
    
  int blocks = 0;
    
  for (int i = 0; i < total; i++) { 
    uint32_t address = count[i];
        
    //invalid
    if (address >= sb->s_blocks_count) {
      free(buffer);
      return 0; 
    }
    get_block_data(sb, address, buffer);

	//valid block
    if (looks_indirect(sb, buffer)){
		blocks++;
	} 
  }

  if (blocks > 0){
	return 1;
  } 
  else{
	return 0;
  }
}
