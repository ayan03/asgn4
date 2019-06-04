#ifndef __MIN_H__
#define __MIN_H__

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include <time.h>
#include "superblock.h"
#include "partition.h"
#include "inode.h"

#define MINIX_MAGIC 19802
#define KILOBYTE 1024
#define MINGET_PROG 0
#define MINLS_PROG 1
#define ROOT_DIR 1
#define MINIX_PARTITION 129
#define PARTITION_TABLE_LOC 446


void print_help(int prog);
void shutdown_help(FILE *fp);
FILE *read_image(char *imagefile, superblock *s_block, int vflag);
int read_inode(FILE *fp, superblock *s_block, inode *inode, int i_num);
int read_partition(FILE *fp, superblock *s_block, pt_entry* partition,
    int p_num);

/* Functions declaration for -v option */
void verb_sblock(superblock *s_block);
void verb_inode(inode *i_node);


#endif /* __MIN_H__ */
