#ifndef FSUTIL_H
#define FSUTIL_H

#if defined(__linux__)
#include <endian.h>
#define SWAP_LE(x, size) do {(x) = htole##size(x);} while(0)
#elif defined(__sun)
#include <sys/byteorder.h>
#define SWAP_LE(x, size) do {(x) = LE_##size(x);} while(0)
#else
#error You are using an unrecognized system.  You will need to
#error define your own SWAP_LE function in e2util.c.
// Put endianness includes and define functions here, then comment out the
// errors above.
#endif


#include <stdio.h>
#include <stdint.h>


// This is the only information we will need from the superblock
struct superblock {
	// We store a pointer to the underlying file here for convenience.
	FILE *file;

	// Filesystem properties

	// Total number of inodes
	uint32_t s_inodes_count;

	// Total number of blocks
	uint32_t s_blocks_count;

	// First block that belongs to the filesystem (i.e. the main superblock)
	uint32_t s_first_data_block;

	// Block size = 1024 << s_log_block_size
	uint32_t s_log_block_size;

	// Blocks in each block group (starting with s_first_data_block)
	uint32_t s_blocks_per_group;

	// Inodes preallocated in each block group
	uint32_t s_inodes_per_group;

	// Filesystem state: s_state == 1 if filesystem was cleanly unmounted
	uint16_t s_state;
};

// This is the information we need from the block group descriptor table
struct bgdesc {
	// Block numbers for block bitmap, inode bitmap, and inode table
	uint32_t bg_block_bitmap;
	uint32_t bg_inode_bitmap;
	uint32_t bg_inode_table;
};

// This is the information we need about inodes
struct inode {
	// Mode (including the `is a directory' bit)
	uint16_t i_mode;

	// Owner
	uint16_t i_uid;

	// File size
	uint32_t i_size;

	// Access, change, modification, and deletion timestamps
	uint32_t i_atime;
	uint32_t i_ctime;
	uint32_t i_mtime;
	uint32_t i_dtime;

	// Blocks of the file: direct...
	uint32_t i_block_d[12];
	// indirect...
	uint32_t i_block_1i;
	// doubly indirect...
	uint32_t i_block_2i;
	// and triply indirect.
	uint32_t i_block_3i;
};


// Functions

// Provided functions
void byteswap_superblock(struct superblock *sb);
void byteswap_bgdesc(struct bgdesc *bg);
void byteswap_inode(struct inode *i);
void byteswap_iblock(struct superblock *sb, char *block);
int blocksize(struct superblock *sb);

void print_superblock(struct superblock *sb);
void print_inode(struct inode *i);

// Most int functions will return 0 on success and 1 on error
int get_superblock(FILE *f, struct superblock *out);
int get_bgdesc(struct superblock *sb, int bg, struct bgdesc *out);
int get_inode(struct superblock *sb, int ino, struct inode *out);

int get_block_data(struct superblock *sb, int blk, char *out);
int print_block_data(struct superblock *sb, int blk);
int get_inode_block(struct superblock *sb, struct inode *i, uint32_t n, char *out);
int print_inode_data(struct superblock *sb, struct inode *i);

// These four return a value directly (no error checking)
int bg_from_blk(struct superblock *sb, int blk);
int blk_within_bg(struct superblock *sb, int blk);
int bg_from_ino(struct superblock *sb, int ino);
int ino_within_bg(struct superblock *sb, int ino);

// These return 1 for yes, 0 for no, -1 for error
int is_block_free(struct superblock *sb, int blk);
int looks_2indirect(struct superblock *sb, char *block);
int looks_indirect(struct superblock *sb, char *block);

#endif
