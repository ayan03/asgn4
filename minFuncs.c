#include "min.h"
#include "partition.h"
#include "superblock.h"
#include "inode.h"

/* Print out the help instructions for -h flag or no flag */
void print_help() {
    printf("usage: minls  [ -v ] [ -p num [ -s num ] ] imagefile");
    printf(" [ path ]\n"); 
    printf("Options:\n");
    printf("    -p   part    --- select partition for filesystem");
    printf(" (default: none)\n");
    printf("    -s   sub     --- select subpartition for filesystem");
    printf(" (default: none)\n");
    printf("    -h   help    --- print usage information and exit\n");
    printf("    -v   verbose --- increase verbosity level\n");
}
