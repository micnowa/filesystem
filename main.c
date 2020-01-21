/**
    Created by Michał Nowaliński on 06.01.2020.
**/

#include <limits.h>
#include <stdlib.h>
#include <time.h>
#include "utils.h"
#include "file_system.h"
#include "help.h"

enum string_code {
    mkfs,
    touch,
    mv,
    cp,
    rm,
    ls,
    rmdisc,
    info
};

int cmd(char *inString) {
    if (!strcmp(inString, "mkfs")) return mkfs;
    if (!strcmp(inString, "touch")) return touch;
    if (!strcmp(inString, "mv")) return mv;
    if (!strcmp(inString, "cp")) return cp;
    if (!strcmp(inString, "rm")) return rm;
    if (!strcmp(inString, "ls")) return ls;
    if (!strcmp(inString, "rmdisc")) return rmdisc;
    if (!strcmp(inString, "info")) return info;
    return INT_MIN;
}

bool print_help(const char *option, const char *help) {
    bool used = false;
    if (strcmp(help, "--help") != 0) {
        return false;
    } else {
        if (!strcmp(option, "mkfs")) {
            mkfs_help();
            used = true;
        }
        if (!strcmp(option, "touch")) {
            touch_help();
            used = true;
        }
        if (!strcmp(option, "mv")) {
            mv_help();
            used = true;
        }
        if (!strcmp(option, "cp")) {
            cp_help();
            used = true;
        }
        if (!strcmp(option, "rm")) {
            rm_help();
            used = true;
        }
        if (!strcmp(option, "ls")) {
            ls_help();
            used = true;
        }
        if (!strcmp(option, "rmdisc")) {
            rmdisc_help();
            used = true;
        }
        if (!strcmp(option, "info")) {
            info_help();
            used = true;
        }
    }
    return used;
}


void mkfs_call(const char *size_arg, const char *block_arg) {
    if (NULL == size_arg || NULL == block_arg) {
        puts("Cannot create ...");
        return;
    }
    int bytes = (int) strtol(size_arg, NULL, 10);
    int block = (int) strtol(block_arg, NULL, 10);
    if (bytes <= 0 || block <= 0) {
        puts("Sizes must be positive number");
        return;
    }
    if (block > bytes) {
        puts("Size of block cannot be greater than size of disc");
        return;
    }
    remove(DISC_NAME);
    create_disc(bytes, block);
}

void touch_call(char *string, char *str2) {
    if (string == NULL || !strcmp(string, "")) {
        puts("Please provide name next time");
        return;
    }
    if (!valid_fname(string)) {
        puts("Invalid file name");
        return;
    }
    printf("Creating file: %s\n", string);

    int bytes = (int) strtol(str2, NULL, 10);
    create_file(extract_name(string), bytes);
}

/**
 * TODO Copy data form src to dest
 * TODO Purge file src
 * TODO Fragmentation is on rm and cp
 *
 * In fact it can be copy called, then rm on src!
 * It's important to save much effort
 *
 * @param src
 * @param dest
 */
void mv_call(const char *src, const char *dest) {

}

/**
 * TODO Copy data from src to dest
 * TODO Fragmentation if needed
 *
 * @param src
 * @param dest
 */
void cp_call(const char *src, const char *dest) {

}

void rm_call(char *file) {
    if (strcmp(file, "*") == 0) {
        puts("removing all files");
        remove_all_files();
    } else {
        descriptor *desc = find(file);
        if (desc != NULL) remove_file(file);
    }
}

void ls_call(char *option) {
    descriptor **desc = load_descriptors_();
    for (int i = 0; i < FS_DESCRIPTOR_NUM; i++) {
        if (desc[i]->size != 0)
            printf("%s %s %d %d\n", desc[i]->name, desc[i]->date, desc[i]->first, desc[i]->size);
    }
}

void rmdisc_call(char *string) {
    remove(DISC_NAME);
}

int main(int argc, char **argv) {
    printf("%lu\n", strlen(current_date()));
    if (argc == 0) return 0;
    for (int i = 0; i < argc; i++) puts(argv[i]);

    if (print_help(argv[1], argv[2])) return 0;
    int command = cmd(argv[1]);
    int opt;
    switch (command) {
        case mkfs:
            puts("Making disc ...");

            char *bytes = (char *) malloc(sizeof(char) * 20);
            char *blocks = (char *) malloc(sizeof(char) * 20);
            while ((opt = getopt(argc, argv, ":s:b:")) != -1) {
                switch (opt) {
                    case 's':
                        strcpy(bytes, optarg);
                        break;
                    case 'b':
                        strcpy(blocks, optarg);
                        break;
                    case ':':
                        printf("option needs a value\n");
                        break;
                    case '?':
                        printf("unknown option: %c\n", optopt);
                        break;
                }
            }
            mkfs_call(bytes, blocks);
            read_disc_info();
            break;
        case touch:
            puts("Creating file ...");
            touch_call(argv[2], argv[3]);
            break;
        case mv:
            puts("Moving file ...");
            char *source = (char *) malloc(sizeof(char) * 32);
            char *destination = (char *) malloc(sizeof(char) * 32);
            while ((opt = getopt(argc, argv, ":s:d:")) != -1) {
                switch (opt) {
                    case 's':
                        strcpy(source, optarg);
                        break;
                    case 'd':
                        strcpy(destination, optarg);
                        break;
                    case ':':
                        printf("option needs a value\n");
                        break;
                    case '?':
                        printf("unknown option: %c\n", optopt);
                        break;
                }
            }
            mv_call(source, destination);
            break;
        case cp:
            puts("Copying file");
            char *src = (char *) malloc(sizeof(char) * 32);
            char *dest = (char *) malloc(sizeof(char) * 32);
            while ((opt = getopt(argc, argv, ":s:d:")) != -1) {
                switch (opt) {
                    case 's':
                        strcpy(source, optarg);
                        break;
                    case 'd':
                        strcpy(destination, optarg);
                        break;
                    case ':':
                        printf("option needs a value\n");
                        break;
                    case '?':
                        printf("unknown option: %c\n", optopt);
                        break;
                }
            }
            cp_call(src, dest);
            break;
        case rm:
            puts("Removing file ...");
            rm_call(argv[2]);
            break;
        case ls:
            puts("listing files ...");
            ls_call(argv[2]);
            break;
        case rmdisc:
            puts("Removing disc ...");
            rmdisc_call(argv[2]);
            break;
        case info:
            puts("Info:");
            read_disc_info();
            break;
        default:
            puts("Command not recognized ...");
            break;
    }

    return 0;
}
