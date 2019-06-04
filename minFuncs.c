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

FILE *read_image(char *imagefile, superblock *s_block, int vflag) {
    FILE *fp = NULL;
    if ((fp = fopen(imagefile, "r")) == NULL) {
        fprintf(stderr, "Error opening image: %s\n", imagefile);
        exit(EXIT_FAILURE);
    }    
    if (fseek(fp, KILOBYTE, SEEK_CUR) == -1) {
        fprintf(stderr, "fseek() error exiting...\n");
        fclose(fp);
        exit(EXIT_FAILURE);
    }
    if (fread(s_block, sizeof(superblock), 1, fp) != 1) {
        fprintf(stderr, "fread() error exiting...\n");
        fclose(fp);
        exit(EXIT_FAILURE);
    }

    /* Check if the magic number matches */
    if (s_block->magic != MINIX_MAGIC) {
        fprintf(stderr, "Bad magic number.  (0x%x)\n", s_block->magic);
        fprintf(stderr, "This doesn't look like a MINIX filesystem\n");
        fclose(fp);
        exit(EXIT_FAILURE);
    }   
    if (vflag) {
        verb_sblock(s_block);
    }
    
    return fp;
}

void verb_sblock(superblock *s_block) {
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
}
