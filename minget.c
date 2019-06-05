#include "min.h"
#include "superblock.h"
#include "partition.h"

int main(int argc, char *argv[]) {
    flags args;
    int opt;
    int opt_ct = 0;
    superblock s_block = { 0 };
    args.partition = -1;
    args.subpartition = -1;
    args.verbose = 0;
    args.image = NULL;
    args.path = NULL;
    args.dstpath = NULL;
    args.fp = NULL;

    /* Handle command line arguments */
    while ((opt = getopt(argc, argv, "vp:s:h")) != -1) {
    /* TODO Switch to strtol instead of atoi to account for partition 0 */
        switch (opt) {
            case 'h':
                print_help(MINGET_PROG);
                break;  
            case 'v':
                args.verbose = 1;
                break;
            case 'p':
                if ((args.partition = atoi(optarg)) < 0) {
                    print_help(MINGET_PROG);
                    exit(EXIT_FAILURE);
                }
                break;
            case 's':
                if ((args.subpartition = atoi(optarg)) < 0 ||
                    args.partition < 0) {
                    print_help(MINGET_PROG);
                    exit(EXIT_FAILURE);
                }
                break;
            case '?':
                print_help(MINGET_PROG);
                exit(EXIT_FAILURE);
                break;
        }
    }
    for (; optind < argc; optind++) {
        if (opt_ct == 0) {
            args.image = argv[optind];
        }
        else if (opt_ct == 1) {
            args.path = argv[optind];
        }
        else if (opt_ct == 2) {
            args.dstpath = argv[optind];
        }
        else {
            print_help(MINGET_PROG);
            exit(EXIT_FAILURE);
        }
        opt_ct++;
    }

    /* Quit if imagefile or srcpath not specified */
    if (args.path == NULL || args.image == NULL) {
        print_help(MINGET_PROG);
        exit(EXIT_FAILURE);
    }

    /* ./minls: print out the help instructions */
    if (argc == 1) {
        print_help(MINGET_PROG);
        return 0;
    }
    read_image(&s_block, &args, MINGET_PROG);
    return 0;
}


