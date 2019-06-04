#include "min.h"
#include "superblock.h"
#include "partition.h"

int main(int argc, char *argv[]) {
    int opt;
    int pval;
    int sval;
    char *image = NULL;
    char *path = NULL;

    /* Handle command line arguments */
    while ((opt = getopt(argc, argv, "vp:s:h")) != -1) {
        switch (opt) {
            case 'h':
                print_help();
                break;  
            case 'p':
                if ((pval = atoi(optarg)) == 0) {
                    fprintf(stderr, "Usage: [ -p num [ -s num ] ]");
                    fprintf(stderr, " image [ path ]\n");
                    exit(EXIT_FAILURE);
                }
                break;
            case 's':
                if ((sval = atoi(optarg)) == 0 || pval == 0) {
                    fprintf(stderr, "Usage: [ -p num [ -s num ] ]");
                    fprintf(stderr, " image [ path ]\n");
                    exit(EXIT_FAILURE);
                }
                break;
            case '?':
                fprintf(stderr, "Usage: [ -p num [ -s num ] ]");
                fprintf(stderr, " image [ path ]\n");
                exit(EXIT_FAILURE);
                break;
        }
    }
    for (; optind < argc; optind++) {
        if (optind == argc - 1) {
            path = argv[optind];
        }
        else if (optind == argc - 2) {
            image = argv[optind];
        }
        else {
            fprintf(stderr, "Usage: [ -p num [ -s num ] ]");
            fprintf(stderr, " image [ path ]\n");
            exit(EXIT_FAILURE);
        }
    }
    /* ./minls: print out the help instructions */
    if (argc == 1) {
        print_help();
    }
    
    return 0;
}
