#ifndef __SUPER_H__
#define __SUPER_H__

#include <stdint.h>

typedef struct __attribute__ ((__packed__)) superblock {
    uint32_t    ninodes; /* number of inodes in this fs */
    uint16_t    pad1; /* padding to line stuff up properly */
    int16_t     i_blocks; /* # of blocks used by inode bit map */
    int16_t     z_blocks; /* # of blocks used by zone bit map */
    uint16_t    firstdata; /* number of first data zone */
    int16_t     log_zone_size; /* log2 of blocks per zone */
    uint16_t    pad2; /* padding to line stuff up properly */
    uint32_t    max_file; /* maximum file size */
    uint32_t    zones; /* number of zones on disk */
    int16_t     magic; /* magic number */
    uint16_t    pad3; /* padding to line suff up properly */
    uint16_t    blocksize; /* block size in bytes */
    uint8_t     subversion; /* filesystem sub-version */
} superblock;

#endif /*__SUPER_H__*/
