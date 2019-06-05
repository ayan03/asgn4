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

void print_mode(uint16_t mode);
void print_help(int prog);
void shutdown_help(FILE *fp);
void read_image(char *imagefile, superblock *s_block, int vflag);
int read_inode(FILE *fp, superblock *s_block, inode *i_node, int i_num);
void read_files(FILE *fp, superblock *s_block, inode *i_node, char *path);

/* Functions declaration for -v option */
void verb_sblock(superblock *s_block);
void verb_inode(inode *i_node);


#endif /* __MIN_H__ */
