#include "utils.h"
#include "file_system.h"

/**
    Created by Michał Nowaliński on 7/1/20.
**/

bool is_positive_int(char *size) {
    if (!(size[0] >= '1' && size[0] <= '9')) return false;
    for (int i = 0; i < strlen(size); i++) {
        if (!(size[i] >= '0' && size[i] <= '9')) return false;
    }
    return true;
}

bool valid_fname(const char *name) {
    if (name[0] == '*') return false;
    if (name[0] == '.') return false;
    return true;
}

bool str_with_quotes(const char *str) {
    int size = (int) strlen(str);
    return str[0] != '\"' && str[size - 1] != '\"';
}


char *extract_name(const char *name) {
    int size = (int) strlen(name);
    if (str_with_quotes(name)) return name;
    char *new_name = (char *) malloc((size - 1) * sizeof(char));
    for (int i = 0; i < size - 2; i++) {
        new_name[i] = name[i + 1];
    }
    new_name[size - 1] = '\0';
    return new_name;
}

char *extract_value(const char *arg, const char *phrase) {
    char *num_str = strstr(arg, phrase);
    char *bytes_str = (char *) malloc(20 * sizeof(char));
    if (num_str != NULL) {
        int j = strlen(phrase);
        int k = 0;
        while (num_str[j] >= '0' && num_str[j] <= '9') {
            bytes_str[k] = num_str[j];
            k++;
            j++;
        }
        puts(bytes_str);
    }
    return bytes_str;
}

char *current_date() {
    char text[100];
    time_t now = time(NULL);
    struct tm *t = localtime(&now);

    strftime(text, sizeof(text) - 1, "%d %m %Y %H:%M", t);
    printf("Current Date: %s\n", text);
    char *str = malloc(sizeof(char) * (strlen(text) + 1));
    strcpy(str, text);
    return str;
}

int calculate_block_number(int disc_size, int block_size) {
    int space = 0; /** file counter, blocks number, block size, descriptors **/
    space += sizeof(int); // counter
    space += sizeof(int); // blocks number
    space += sizeof(int); // block size
    space += FS_DESCRIPTOR_SIZE * FS_DESCRIPTOR_NUM; // initially 100 descriptors
    int remaining_size = disc_size - space;
    remaining_size = (int) (remaining_size / (block_size + FS_NODE_SIZE));
    return remaining_size;
}
