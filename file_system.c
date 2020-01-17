/**
// Created by Michał Nowaliński on 7/1/20.
**/
#include "file_system.h"

void read_disc_info() {
    FILE *file = open_disc("r");
    int size, block, block_number, counter;

    fseek(file, 0, SEEK_SET);
    fread(&size, 4, 1, file);

    fseek(file, 4, SEEK_SET);
    fread(&counter, 4, 1, file);

    fseek(file, 8, SEEK_SET);
    fread(&block_number, 4, 1, file);

    fseek(file, 12, SEEK_SET);
    fread(&block, 4, 1, file);

    printf("SIZE: %d\n", size);
    printf("COUNTER: %d\n", counter);
    printf("BLOCK NUMBER: %d\n", block_number);
    printf("BLOCK SIZE: %d\n", block);
}

void create_disc(int disc_size, int block_size) {
    FILE *file = NULL;
    file = fopen(DISC_NAME, "w");
    if (file == NULL) {
        puts("Unable to reach the disc ...");
        return;
    }
    ftruncate(fileno(file), disc_size);

    /** Writing disc size**/
    fwrite(&disc_size, FS_DISC_SIZE_SIZE, 1, file);

    /** Writing files counter **/
    int offset = FS_DISC_SIZE_SIZE;
    int files_num = 0;
    fseek(file, offset, SEEK_SET);
    fwrite(&files_num, FS_COUNTER_SIZE, 1, file);

    /** Writing block number **/
    int blocks_num = calculate_block_number(disc_size, block_size);
    offset += FS_COUNTER_SIZE;
    fseek(file, offset, SEEK_SET);
    fwrite(&blocks_num, FS_BLOCK_NUMBER_SIZE, 1, file);

    /** Writing block size **/
    offset += FS_BLOCK_NUMBER_SIZE;
    fseek(file, offset, SEEK_SET);
    fwrite(&block_size, FS_COUNTER_SIZE, 1, file);

    /** Creating empty descriptors, 100 at the begin **/
    descriptor desc = (descriptor) {.date = "", .name = "", .first = -1, .size=0};
    offset += FS_BLOCK_SIZE_SIZE;
    fseek(file, offset, SEEK_SET);
    for (int i = 0; i < FS_DESCRIPTOR_NUM; i++) {
        fseek(file, offset, SEEK_SET);
        fwrite(&desc, FS_DESCRIPTOR_SIZE, 1, file);
        offset += FS_DESCRIPTOR_SIZE;
    }

    /** Creating empty nodes **/
    node nd = (node) {.next = -1, .has_data  = false};
    for (int i = 0; i < blocks_num; i++) {
        fseek(file, offset, SEEK_SET);
        fwrite(&nd, FS_NODE_SIZE, 1, file);
        offset += FS_NODE_SIZE;
    }

    /** Creating blocks **/
    void *ptr = calloc(blocks_num, block_size);
    for (int i = 0; i < FS_DESCRIPTOR_NUM; i++) {
        fseek(file, offset, SEEK_SET);
        fwrite(ptr, block_size, 1, file);
        offset += block_size;
    }
    fclose(file);
}

descriptor **load_descriptors() {
    FILE *file = open_disc("r");
    int offset = FS_FIRST_DESCRIPTOR;
    descriptor **desc = malloc(sizeof(descriptor *) * FS_DESCRIPTOR_NUM);
    for (int i = 0; i < FS_DESCRIPTOR_NUM; i++) {
        desc[i] = malloc(sizeof(descriptor));
        fseek(file, offset + FS_DESCRIPTOR_SIZE * i, SEEK_SET);
        fread(desc[i], FS_DESCRIPTOR_SIZE, 1, file);
        //printf("%s %s %d %d\n", desc[i]->name, desc[i]->date, desc[i]->first, desc[i]->size);
    }
    fclose(file);

    return desc;
}

node **load_nodes() {
    FILE *file = open_disc("r");
    int offset = FS_DISC_SIZE_SIZE + FS_COUNTER_SIZE;
    int block_number;
    fseek(file, offset, SEEK_SET);
    fread(&block_number, FS_BLOCK_NUMBER_SIZE, 1, file);
    offset = FS_FIRST_DESCRIPTOR + FS_DESCRIPTOR_NUM * FS_DESCRIPTOR_SIZE;
    node **nds = malloc(sizeof(node *) * block_number);
    for (int i = 0; i < block_number; i++) {
        nds[i] = malloc(sizeof(node));
        fseek(file, offset + FS_NODE_SIZE * i, SEEK_SET);
        fread(nds[i], FS_NODE_SIZE, 1, file);
        //printf("%u %s\n", nds[i]->next, nds[i]->has_data ? "true" : "false");
    }
    fclose(file);
    return nds;
}

void create_file(const char *fname, int bytes) {
    if (!enough_nodes(bytes)) {
        puts("Not enough space ...");
        return;
    }
    remove_file(fname);

    int descriptor_position = -1;
    descriptor **desc = load_descriptors();
    node **nd = load_nodes();
    int block_size = load_block_size();
    int blocks_to_write = !(bytes % block_size) ? bytes / block_size : bytes / block_size + 1;

    /** Looking for descriptor **/
    for (int i = 0; i < FS_DESCRIPTOR_NUM; i++) if (desc[i]->first == -1) {
        descriptor_position = i;
        break;
    }
    if (descriptor_position == -1) {
        puts("Exceeded files limit ...");
        return;
    }

    int current = 0, next, node_number = load_block_number();
    FILE *file = open_disc("wb");
    int offset = FS_FIRST_DESCRIPTOR;
    offset += FS_DESCRIPTOR_SIZE * descriptor_position;
    while (!nd[current]->has_data) current++;
    char name_tab[FS_NAME_SIZE], date_tab[FS_DATE_SIZE];
    strcpy(name_tab, fname);
    strcpy(date_tab, current_date());
    descriptor descriptor1 = (descriptor) {.first = current, .name = name_tab, .size = bytes, .date = date_tab};

    /** Writing descriptor **/
    fseek(file, offset, SEEK_SET);
    fwrite(&descriptor1, FS_DESCRIPTOR_SIZE, 1, file);

    /** Writing node**/
    offset = node_offset(current);
    fclose(file);
    int block_offs =  block_offset(current);
    file = open_disc("wb");

    node node_to_write;
    while (blocks_to_write != 0) {
        fseek(file, offset, SEEK_SET);
        node_to_write.has_data = true;
        while (nd[current]->has_data) current++;
        node_to_write.next = current;
        fwrite(&node_to_write, FS_NODE_SIZE, 1, file);
        blocks_to_write--;
    }
}

void remove_all_files() {
    FILE *file = NULL;
    file = fopen(DISC_NAME, "r");
    if (file == NULL) {
        puts("Couldn't open the disc ...");
        return;
    }

    int disc_size, block_size;
    fread(&disc_size, FS_DISC_SIZE_SIZE, 1, file);
    fseek(file, FS_DISC_SIZE_SIZE + FS_COUNTER_SIZE + FS_BLOCK_NUMBER_SIZE, SEEK_SET);
    fread(&block_size, FS_BLOCK_SIZE_SIZE, 1, file);

    fclose(file);
    remove(DISC_NAME);
    create_disc(disc_size, block_size);
}

void remove_file(const char *name) {
    /*descriptor **desc = load_descriptors();
    for (int i = 0; i < FS_DESCRIPTOR_NUM; i++) {
        if (strcmp(desc[i]->name, name) == 0) puts("wow");
    }*/
    //fclose(file);
}

void move_file(const char *source, const char *dest) {}

descriptor *find(const char *file) {
    descriptor **desc = load_descriptors();
    for (int i = 0; i < FS_DESCRIPTOR_NUM; i++) {
        if (strcmp(desc[i]->name, file) == 0) {
            return desc[i];
        }
    }
    return NULL;
}

int *load_nodes_numbers(int *block_number, descriptor desc, int block_size) {
    *block_number = (int) (desc.size / block_size) + 1;
    int *tab = malloc(sizeof(int) * (*block_number - 1));
    tab[0] = desc.first;

    FILE *file = NULL;
    file = fopen(DISC_NAME, "r");
    if (file == NULL) {
        puts("Couldn't open the disc ...");
        return NULL;
    }

    node nd;
    int offset = FS_FIRST_NODE + FS_NODE_SIZE * desc.first;
    for (int i = 1; i < *block_number; i++) {
        fseek(file, offset, SEEK_SET);
        fread(&nd, FS_NODE_SIZE, 1, file);
        tab[i] = nd.next;
        offset = FS_FIRST_NODE + FS_NODE_SIZE * nd.next;
    }
    fclose(file);
    return tab;
}
