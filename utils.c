#include "utils.h"
#include "file_system.h"

/**
    Created by Michał Nowaliński on 7/1/20.
**/

FILE *open_disc(const char *mode) {
    FILE *file = NULL;
    file = fopen(DISC_NAME, mode);
    if (file == NULL) {
        puts("Unable to reach the disc ...");
        return NULL;
    } else return file;
}


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


char *current_date() {
    char text[100];
    time_t now = time(NULL);
    struct tm *t = localtime(&now);

    strftime(text, sizeof(text) - 1, "%d %m %Y %H:%M", t);
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

int node_offset(int number) {
    return FS_FIRST_NODE + number * FS_NODE_SIZE;
}

int block_offset(int number, int block_number, int block_size) {
    int offset = FS_FIRST_NODE + FS_NODE_SIZE * block_number;
    offset += block_size * number;
    return offset;
}

int load_disc_size() {
    FILE *file = NULL;
    file = fopen(DISC_NAME, "rb");
    if (file == NULL) {
        puts("Unable to reach the disc ...");
        return 0;
    }

    int disc_size;
    fread(&disc_size, FS_DISC_SIZE_SIZE, 1, file);
    fclose(file);
    return disc_size;
}

int load_file_counter() {
    FILE *file = NULL;
    file = fopen(DISC_NAME, "rb");
    if (file == NULL) {
        puts("Unable to reach the disc ...");
        return 0;
    }

    int file_counter;
    fseek(file, FS_DISC_SIZE_SIZE, SEEK_SET);
    fread(&file_counter, FS_COUNTER_SIZE, 1, file);
    fclose(file);
    return file_counter;
}

int load_block_number() {
    FILE *file = fopen(DISC_NAME, "r");
    if (file == NULL) {
        puts("Unable to reach the disc ...");
        return 0;
    }

    int block_number;
    fseek(file, FS_DISC_SIZE_SIZE + FS_COUNTER_SIZE, SEEK_SET);
    fread(&block_number, FS_BLOCK_NUMBER_SIZE, 1, file);
    fclose(file);
    return block_number;
}

int load_block_size() {
    FILE *file = fopen(DISC_NAME, "r");
    if (file == NULL) {
        puts("Unable to reach the disc ...");
        return 0;
    }

    int block_size;
    fseek(file, FS_DISC_SIZE_SIZE + FS_COUNTER_SIZE + FS_BLOCK_NUMBER_SIZE, SEEK_SET);
    fread(&block_size, FS_BLOCK_SIZE_SIZE, 1, file);
    fclose(file);
    return block_size;
}

bool enough_nodes(int bytes) {
    node **nd = load_nodes_();
    int node_number = load_block_number();
    int block_size = load_block_size();
    int counter = 0;
    for (int i = 0; i < node_number; i++) {
        if (!nd[i]->has_data) counter++;
    }
    return block_size * node_number >= bytes;
}

void increment_counter(bool plus) {
    FILE *file = open_disc("r+");
    int size;
    fseek(file, FS_DISC_SIZE_SIZE, SEEK_SET);
    fread(&size, FS_COUNTER_SIZE, 1, file);
    size = plus ? size + 1 : size - 1;
    fseek(file, FS_DISC_SIZE_SIZE, SEEK_SET);
    fwrite(&size, FS_COUNTER_SIZE, 1, file);
    fclose(file);
}

