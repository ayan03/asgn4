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
FILE *read_image(char *imagefile, superblock *s_block, int vflag) {
    inode i_node = { 0 };
    FILE *fp = NULL;
    if ((fp = fopen(imagefile, "r")) == NULL) {
        fprintf(stderr, "Error opening image: %s\n", imagefile);
        exit(EXIT_FAILURE);
    }    
    if (fseek(fp, KILOBYTE, SEEK_CUR) == -1) {
        fprintf(stderr, "fseek() error exiting...\n");
        shutdown_help(fp);
    }
    if (fread(s_block, sizeof(superblock), 1, fp) != 1) {
        fprintf(stderr, "fread() error exiting...\n");
        shutdown_help(fp);
    }

    /* Check if the magic number matches */
    if (s_block->magic != MINIX_MAGIC) {
        fprintf(stderr, "Bad magic number.  (0x%x)\n", s_block->magic);
        fprintf(stderr, "This doesn't look like a MINIX filesystem\n");
        shutdown_help(fp);
    }   
    read_inode(fp, s_block, &i_node, ROOT_DIR);

    if (vflag) {
        verb_sblock(s_block);
        verb_inode(&i_node);
    }
    
    return fp;
}

/* TODO Incomplete only works for root node case */
/* Read in given i_node always start at root */
int read_inode(FILE *fp, superblock *s_block, inode *i_node, int i_num) {
    uint32_t inode_loc = (2 + s_block->i_blocks + s_block->z_blocks) *
s_block->blocksize;
    if (fseek(fp, inode_loc, SEEK_SET) == -1) {
        fprintf(stderr, "fseek() error exiting...\n");
        shutdown_help(fp);
    }
    if (fread(i_node, sizeof(inode), 1, fp) != 1) {
        fprintf(stderr, "fread() error exiting...\n");
        shutdown_help(fp);
    }
    printf("uid %d\n", i_node->mode);
    return 0;
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







