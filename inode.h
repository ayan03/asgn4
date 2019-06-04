#ifndef __INODE__
#define __INODE__

#include <stdint.h>
#define DIRECT_ZONES 7


typedef struct __attribute__ ((__packed__)) inode {
    uint16_t    mode;
    uint16_t    links; /* number of links to file */
    uint16_t    uid;
    uint16_t    gid;
    uint32_t    size;
    int32_t     atime; /* Last accessed time */
    int32_t     mtime; /* Last modified time */
    int32_t     ctime; /* Created time */
    uint32_t    zone[DIRECT_ZONES];
    uint32_t    indirect;
    uint32_t    double_indirect;
    uint32_t    unused;
} inode;

typedef struct __attribute__ ((__packed__)) dirent {
    uint32_t    inode; /* inode number */
    u_char       name[60]; /* filename (nul-terminated if space available) */
} dirent;

#endif /* __INODE__ */
