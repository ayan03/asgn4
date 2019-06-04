#ifndef __MIN_H__
#define __MIN_H__

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include "superblock.h"
#include "partition.h"
#include "inode.h"

#define MINIX_MAGIC 19802
#define KILOBYTE 1024
#define MINGET_PROG 0
#define MINLS_PROG 1
#define MINIX_PARTITION 129


void print_help(int prog);
FILE *read_image(char *imagefile, superblock *s_block, int vflag);
void verb_sblock(superblock *s_block);


#endif /* __MIN_H__ */
