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

void read_image(superblock *s_block, flags *args) {
    int i = 0;
    int j = 0;
    int match = 0;
    int zone_num = 0;
    char *token;
    inode i_node = { 0 };
    uint64_t p_off = 0;
    dirent *dir = NULL;
    char **path_arr = NULL;
    uint32_t zone_size = 0;
    char tmp_path[MAX_PATH];

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
        fprintf(stderr, "Bad magic number.  (0x%04x)\n", s_block->magic);
        fprintf(stderr, "This doesn't look like a MINIX filesystem\n");
        shutdown_help(args->fp);
    }   
    zone_size = s_block->blocksize << s_block->log_zone_size;

    /* Read root inode */
    read_inode(args->fp, s_block, &i_node, ROOT_DIR, p_off);

    /* TODO ADD PATH */
    /* Allocate space for path */
    if (args->path != NULL) {
        if ((path_arr = calloc(args->path_ct, sizeof(char*))) == NULL) {
            fprintf(stderr, "Calloc() error exiting...\n");
            shutdown_help(args->fp);
        }
        for (i = 0; i < args->path_ct; i++) {
            if ((path_arr[i] = calloc(FLENGTH + 1, sizeof(char))) == NULL) {
                fprintf(stderr, "Calloc() error exiting...\n");
                shutdown_help(args->fp);
            }
        }
        i = 0;
        strcpy(tmp_path, args->path);
        token = strtok(tmp_path, "/");
        while (token != NULL) {
            strcpy(path_arr[i], token);
            i++;
            token = strtok(NULL, "/");
        }
        
        if ((dir = calloc(1, sizeof(dirent))) == NULL) {
            fprintf(stderr, "Calloc failure exiting...\n");
            shutdown_help(args->fp);
            /* Add frees */
        }
        /* TODO Traverse from root to end path */

        for (i = 0; i < args->path_ct; i++) {
            if (match != i) {
                fprintf(stderr, "Bad path: %s\n", args->path);
                shutdown_help(args->fp);
            }
            zone_num = 0;
            fseek(args->fp, zone_size * i_node.zone[zone_num] + p_off, 
SEEK_SET);
            for (j = 0; j < i_node.size / sizeof(dirent); j++) {
                fread(dir, sizeof(dirent), 1, args->fp);
                /* If file names match */
                if (!strcmp((char*)dir->name, path_arr[i])) {
                    read_inode(args->fp, s_block, &i_node, dir->inode, p_off);
                    match++;
                    break;
                }
                /* Move to next direct zone if possible */
                if (((j + 1) * sizeof(dirent) % zone_size) == 0) {
                    if (zone_num < DIRECT_ZONES - 1) {
                        zone_num++;
                        fseek(args->fp, zone_size * i_node.zone[zone_num] + 
p_off, SEEK_SET);    
                    }
                }
                /* TODO INDIRECT ZONES */
            }
        }
    }


    if (args->verbose) {
        verb_sblock(s_block);
        verb_inode(&i_node);
    }

    /* Print out path if specified */
    if (S_ISDIR(i_node.mode)) {
        
        if (args->path != NULL) {
            printf("%s/:\n", args->path);
        }
        else {
            printf("/:\n");
        }
    }
    
    /* Print out entire directory information */
    read_files(args, s_block, &i_node, p_off);

    /* Free Path if applicable */
    if (path_arr) {
        for (i = 0; i < args->path_ct; i++) {
            free(path_arr[i]);
        }
        free(path_arr);
    }
    fclose(args->fp);
}

/* Print out the files in given directory */
void read_files(flags *args, superblock *s_block, inode *i_node, 
uint64_t p_off) {
    int i = 0;
    FILE *d_fp;
    dirent *dir = NULL;
    inode *other_node;
    int zone_num = 0;
    uint32_t zone_size = s_block->blocksize << s_block->log_zone_size;
    d_fp = fopen(args->image, "r");
    if ((dir = calloc(1, sizeof(dirent))) == NULL) {
        fprintf(stderr, "Calloc failure exiting...\n");
        fclose(d_fp);
        shutdown_help(args->fp);
    }
    
    if ((other_node = calloc(1, sizeof(inode))) == NULL) {
        fprintf(stderr, "Calloc failure exiting...\n");
        fclose(d_fp);
        shutdown_help(args->fp);
    }
    /* Working with a file */
    if (!S_ISDIR(i_node->mode)) {
        print_mode(i_node->mode);
        printf("    %d   %s\n", i_node->size ,args->path);
        return;
    }

    /* Move to first direct zone */
    fseek(args->fp, zone_size * i_node->zone[zone_num] + p_off, SEEK_SET);

    /* Iterate through number of files */
    for (i = 0; i < i_node->size / sizeof(dirent); i++) {
        fread(dir, sizeof(dirent), 1, args->fp);
        /* Print out file information */
        if (dir->inode != 0) {
            read_inode(d_fp, s_block, other_node, dir->inode, p_off);
            print_mode(other_node->mode);
            printf("    %d   %s\n", other_node->size ,dir->name);
        }
        /* Check to see if you need to move to the next zone */
        if (((i + 1) * sizeof(dirent) % zone_size) == 0) {
            /* Move to next direct zone if possible */
            if (zone_num < DIRECT_ZONES - 1) {
                zone_num++;
                fseek(args->fp, zone_size * i_node->zone[zone_num] + p_off,
SEEK_SET);
            }
            /* TODO IMPLEMENT INDIRECT */
        }
        
    }

    /* Free all data being used */
    free(dir);
    free(other_node);
    fclose(d_fp);
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

void print_mode(uint16_t mode) {
    if (S_ISDIR(mode)) {
        printf("d");
    }
    else {
        printf("-");
    }
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

