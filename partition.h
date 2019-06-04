#ifndef __PARTITION_H__
#define __PARTITION_H__

#include <stdint.h>

typedef struct pt_entry {
    uint8_t     bootind;            /* Boot magic number (0x80 if bootable) */
    uint8_t     start_head;         /* Start of partition in CHS */
    uint8_t     start_sec_cyl[2];   /* See note on sec_cyl addressing */
    uint8_t     type;               /* Type of Partition (0x81 is MINIX) */
    uint8_t     end_head;           /* End of partition in CHS */
    uint8_t     end_sec_cyl[2];     /* See note on sec_cyl addressing */
    uint32_t    lFirst;             /* First sector (LBA addressing) */
    uint32_t    size;               /* size of partition (in sectors */
} pt_entry;

#endif /* __PARTITION_H__ */
