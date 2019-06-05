#include "min.h"
#include "superblock.h"
#include "partition.h"

int main(int argc, char *argv[]) {
    int opt;
    int opt_ct = 0;
    int pval = 0;
    int sval = 0;
    int vflag = 0;
    char *image = NULL;
    char *path = NULL;
    superblock s_block = { 0 };

    /* Handle command line arguments */
    while ((opt = getopt(argc, argv, "vp:s:h")) != -1) {
    /* TODO Switch to strtol instead of atoi to account for partition 0 */
        switch (opt) {
            case 'h':
                print_help(MINLS_PROG);
                break;  
            case 'v':
                vflag = 1;
                break;
            case 'p':
                if ((pval = atoi(optarg)) == 0) {
                    print_help(MINLS_PROG);
                    exit(EXIT_FAILURE);
                }
                break;
            case 's':
                if ((sval = atoi(optarg)) == 0 || pval == 0) {
                    print_help(MINLS_PROG);
                    exit(EXIT_FAILURE);
                }
                break;
            case '?':
                print_help(MINLS_PROG);
                exit(EXIT_FAILURE);
                break;
        }
    }
    for (; optind < argc; optind++) {
        if (opt_ct == 0) {
            image = argv[optind];
        }
        else if (opt_ct == 1) {
            path = argv[optind];
        }
        else {
            print_help(MINLS_PROG);
            exit(EXIT_FAILURE);
        }
    }

    /* ./minls: print out the help instructions */
    if (argc == 1) {
        print_help(MINLS_PROG);
        return 0;
    }
    read_image(image, &s_block, vflag);
    return 0;
}
