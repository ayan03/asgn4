#include "min.h"

/* Print out the help instructions for -h flag or no flag */

void print_help(int prog) {
    if (prog == MINLS_PROG) {
        printf("usage: minls   [ -v ] [ -p part [ -s subpart ] ] imagefile");
        printf(" [ path ]\n"); 
    }
    else { 
        printf("usage: minget  [ -v ] [ -p part [ -s subpart ] ] imagefile");
        printf(" srcpath [ dstpath]\n");
    }
    printf("Options:\n");
    printf("    -p   part    --- select partition for filesystem");
    printf(" (default: none)\n");
    printf("    -s   sub     --- select subpartition for filesystem");
    printf(" (default: none)\n");
    printf("    -h   help    --- print usage information and exit\n");
    printf("    -v   verbose --- increase verbosity level\n");
}

/* Shutdown Error case */
void shutdown_help(FILE *fp) {
    fclose(fp);
    exit(EXIT_FAILURE);
}

void read_image(superblock *s_block, flags *args, int prog) {
    inode i_node = { 0 };
    uint64_t p_off = 0;
    if ((args->fp = fopen(args->image, "r")) == NULL) {
        fprintf(stderr, "Error opening image: %s\n", args->image);
        exit(EXIT_FAILURE);
    }    

    if (args->partition != -1) {
        read_partition(args, &p_off, args->partition);
    }

    if (args->subpartition != -1) {
        read_partition(args, &p_off, args->subpartition);
    }

    if (fseek(args->fp, KILOBYTE, SEEK_CUR) == -1) {
        fprintf(stderr, "fseek() error exiting...\n");
        shutdown_help(args->fp);
    }
    if (fread(s_block, sizeof(superblock), 1, args->fp) != 1) {
        fprintf(stderr, "fread() error exiting...\n");
        shutdown_help(args->fp);
    }

    /* Check if the magic number matches */
    if (s_block->magic != MINIX_MAGIC) {
        fprintf(stderr, "Bad magic number.  (0x%x)\n", s_block->magic);
        fprintf(stderr, "This doesn't look like a MINIX filesystem\n");
        shutdown_help(args->fp);
    }   
    /* Read root inode */
    read_inode(args->fp, s_block, &i_node, ROOT_DIR, p_off);


    if (args->verbose) {
        verb_sblock(s_block);
        verb_inode(&i_node);
    }
    if(prog == MINLS_PROG) {
        read_files(args, s_block, &i_node, p_off);
    }
    else {
        transfer_file(s_block, &i_node, args, &p_off);
    }
}

/* Print out the files in given directory */
void read_files(flags *args, superblock *s_block, inode *i_node, 
uint64_t p_off) {
    int i = 0;
    FILE *d_fp;
    dirent *dir = NULL;
    inode *other_node;
    uint32_t zone_size = s_block->blocksize << s_block->log_zone_size;
    d_fp = fopen(args->image, "r");
    if ((dir = calloc(1, sizeof(dirent))) == NULL) {
        fprintf(stderr, "Calloc failure exiting...\n");
        shutdown_help(args->fp);
    }
    
    if ((other_node = calloc(1, sizeof(inode))) == NULL) {
        fprintf(stderr, "Calloc failure exiting...\n");
        shutdown_help(args->fp);
    }

    fseek(args->fp, zone_size * i_node->zone[0] + p_off, SEEK_SET);

    for (i = 0; i < i_node->size / sizeof(dirent); i++) {
        fread(dir, sizeof(dirent), 1, args->fp);
        if (dir->inode != 0) {
            read_inode(d_fp, s_block, other_node, dir->inode, p_off);
            print_mode(other_node->mode);
            printf("    %d   %s\n", other_node->size ,dir->name);
        }
    }
    free(dir);
    free(other_node);
}

/* Read in given i_node always start at root */
int read_inode(FILE *fp, superblock *s_block, inode *i_node, int i_num,
uint64_t p_off) {
    uint32_t inode_loc = (2 + s_block->i_blocks + s_block->z_blocks) *
s_block->blocksize;
    inode_loc += (i_num - 1) * sizeof(inode);
    if (fseek(fp, inode_loc + p_off, SEEK_SET) == -1) {
        fprintf(stderr, "fseek() error exiting...\n");
        shutdown_help(fp);
    }
    if (fread(i_node, sizeof(inode), 1, fp) != 1) {
        fprintf(stderr, "fread() error exiting...\n");
        shutdown_help(fp);
    }
    return 0;
}


int read_partition(flags *args, uint64_t *p_off, int p_num) {
    uint8_t boot_sector[512];
    pt_entry *table;
    pt_entry *partition;
    fseek(args->fp, *p_off, SEEK_SET);
    fread(boot_sector, 512, 1, args->fp);
    if(boot_sector[510] != 85 || boot_sector[511] != 170) {
        fprintf(stderr, "Bad partition exiting...\n");
        shutdown_help(args->fp);
    }
    
    /* Get the partition table from the boot sector and print if verbose*/
    table = (pt_entry*)(boot_sector + PARTITION_TABLE_LOC);
    if (args->verbose) { 
        printf("Print the table TODO\n");
    }

    /* Index to desired partition in the table */
    partition = table + p_num;

    /* Check to see if the desired partition is valid */
    if(partition->type != MINIX_TYPE) {
        fprintf(stderr, "Not a valid partition found in the table\n");
        shutdown_help(args->fp);
    }
    
    /* Move offset to the first sector of LBA adressing */
    *p_off = partition->lFirst * 512;
    return 0;
} 

uint32_t min(uint32_t a, uint32_t b) {
    if (a < b) {
        return a;
    }
    return b;
}
void read_zone(flags *args, uint8_t *buffer, uint32_t available, 
     uint64_t offset, uint32_t zone) {
     if(zone != 0) {
         fseek(args->fp, offset, SEEK_SET);
         fread(buffer, available, 1, args->fp);
     }
     else {
         memset(buffer, 0, available);
     }
}

int output_file(flags *args, uint8_t *buffer, uint32_t size){
    FILE *dst;
    if (args->dstpath) {
        dst = fopen(args->dstpath, "w");
        if (!dst) {
            fprintf(stderr, "Couldn't open %s for writing\n", args->dstpath);
            return 1;
        }
        fwrite(buffer, size, 1, dst);
        fclose(dst);
    }
    else {
        /* write contents of file to stdout */
        write(0, buffer, size);
    }        
    return 0;
}
void transfer_file(superblock *s_block, inode *i_node,
    flags *args, uint64_t*p_off) {
    int zone_size = s_block->blocksize << s_block->log_zone_size;
    uint8_t *buffer = (uint8_t*)calloc(i_node->size,sizeof(uint8_t));
    uint32_t cursor = 0;
    uint32_t remaining = i_node->size;
    uint32_t *indirect_zone = (uint32_t*)calloc(zone_size, 1);
    uint32_t *dindirect_zone = (uint32_t*) calloc(zone_size, 1);
    int i;
    int j;
    int result;
    uint32_t available = 0;
    
    /* Read in all the direct zones necessary */ 
    for (i = 0; i < DIRECT_ZONES; i++) {
        /*get the maximum amount of bytes that should be read from the zone */
        available = min(remaining, zone_size);

        /* read in zone data */
        read_zone(args, buffer + cursor, available, 
            i_node->zone[i] * zone_size + *p_off, i_node->zone[i]);

        /* update variables for continued  reading */          
        remaining -= available;
        cursor += available; 
        if (remaining == 0) {
            break;
        }
    }
    
    if (remaining != 0) {
       /* get the indirect zones */
        fseek(args->fp, i_node->indirect * zone_size + *p_off, SEEK_SET);
        fread(indirect_zone, zone_size, 1, args->fp);
        for (i = 0; i < zone_size / sizeof(uint32_t); i++) {
            available = min(remaining, zone_size);
            read_zone(args, buffer + cursor, available, 
                indirect_zone[i] * zone_size + *p_off, indirect_zone[i]);
            remaining -= available;
            cursor += available;
            if (remaining == 0) {
                break;
            }
        }
    }

    if (remaining != 0) {
        /* get the double indirect zones */
        fseek(args->fp, i_node->double_indirect * zone_size + *p_off, SEEK_SET);
        fread(dindirect_zone, zone_size, 1, args->fp);
        for (i = 0; i < zone_size / sizeof(uint32_t); i++) {
            /* get the indirect zone */
            fseek(args->fp, dindirect_zone[i] * zone_size + *p_off, SEEK_SET);
            fread(indirect_zone, zone_size, 1, args->fp);
            for (j = 0; j < zone_size /sizeof(uint32_t); i++) {
                available = min(remaining, zone_size);
                read_zone(args, buffer + cursor, available,
                    indirect_zone[j] * zone_size + *p_off, indirect_zone[j]);
                remaining -= available;
                cursor += available;
                if(remaining == 0) {
                    break;
                }
            }
        }    
    }    
    result = output_file(args, buffer, i_node->size);
    free(buffer);
    free(indirect_zone);
    free(dindirect_zone);
    if (result) {
        /* something went wrong when outputing to the file */
        shutdown_help(args->fp);
    }
}    


void print_mode(uint16_t mode) {
    /* Print permission bits for owner */
    if (mode & 0400) {
        printf("r");
    }
    else {
        printf("-");
    }
    if (mode & 0200) {
        printf("w");
    }
    else {
        printf("-");
    }
    if (mode & 0100) {
        printf("x");
    }
    else {
        printf("-");
    }
    /* Print permission bits for group */
    if (mode & 0040) {
        printf("r");
    }
    else {
        printf("-");
    }
    if (mode & 0020) {
        printf("w");
    }
    else {
        printf("-");
    }
    if (mode & 0010) {
        printf("x");
    }
    else {
        printf("-");
    }
    /* Print permission bits for other */
    if (mode & 0004) {
        printf("r");
    }
    else {
        printf("-");
    }
    if (mode & 0002) {
        printf("w");
    }
    else {
        printf("-");
    }
    if (mode & 0001) {
        printf("x");
    }
    else {
        printf("-");
    }
}

/* Print out superblock information */
void verb_sblock(superblock *s_block) {
/* TODO Formatting */
    uint32_t zone_size = s_block->blocksize << s_block->log_zone_size;
    printf("Stored Fields:\n");
    printf("  ninodes   %10d\n", s_block->ninodes);    
    printf("  i_blocks  %10d\n", s_block->i_blocks);
    printf("  z_blocks  %10d\n", s_block->z_blocks);
    printf("  firstdata %10d\n", s_block->firstdata);
    printf("  log_zone_size     %d (zone size: %d)\n", 
    s_block->log_zone_size, zone_size);
    printf("  max_file  %10" PRIu32 "\n", s_block->max_file);
    printf("  magic     %10x\n", s_block->magic);
    printf("  zones     %10d\n", s_block->zones);
    printf("  blocksize %10d\n", s_block->blocksize);
    printf("  subversion%10d\n", s_block->subversion);
    printf("\n");
}

/* Print out the inode information */
void verb_inode(inode *i_node) {
/* TODO Formatting and add permissions for mode */
    time_t a_time = i_node->atime;
    time_t m_time = i_node->mtime;
    time_t c_time = i_node->ctime;
    printf("File inode:\n");
    printf("  uint16_t mode         0x%x\n", i_node->mode);
    printf("  uint16_t links        %d\n", i_node->links);
    printf("  uint16_t uid          %d\n", i_node->uid);
    printf("  uint16_t gid          %d\n", i_node->gid);
    printf("  uint32_t size         %d\n", i_node->size);
    printf("  uint32_t atime        %" PRIu32 "---%s", i_node->atime, 
ctime(&a_time));
    printf("  uint32_t mtime        %" PRIu32 "---%s", i_node->mtime, 
ctime(&m_time));
    printf("  uint32_t ctime        %" PRIu32 "---%s", i_node->ctime, 
ctime(&c_time));
    printf("\n");
    printf("  Direct zones:\n");
    printf("                zone[0]     =       %d\n", i_node->zone[0]);
    printf("                zone[1]     =       %d\n", i_node->zone[1]);
    printf("                zone[2]     =       %d\n", i_node->zone[2]);
    printf("                zone[3]     =       %d\n", i_node->zone[3]);
    printf("                zone[4]     =       %d\n", i_node->zone[4]);
    printf("                zone[5]     =       %d\n", i_node->zone[5]);
    printf("                zone[6]     =       %d\n", i_node->zone[6]);
    printf("  uint32_t      indirect            %d\n", i_node->indirect);
    printf("  uint32_t      double              %d\n", 
i_node->double_indirect);
}







