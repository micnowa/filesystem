/**
// Created by Michał Nowaliński on 7/1/20.
**/
#include "file_system.h"

void create_disc(int disc_size, int block_size) {
    FILE *file = NULL;
    file = fopen(DISC_NAME, "w");
    if (file == NULL) {
        puts("Unable to reach the disc ...");
        return;
    }
    ftruncate(fileno(file), disc_size);

    /** Writing disc size**/
    fwrite(&disc_size, FS_DISC_SIZE, 1, file);

    /** Writing files counter **/
    int offset = FS_DISC_SIZE;
    int files_num = 0;
    fseek(file, offset, SEEK_SET);
    fwrite(&files_num, FS_COUNTER_SIZE, 1, file);

    /** Writing block number **/
    int blocks_num = calculate_block_number(disc_size, block_size);
    offset += FS_COUNTER_SIZE;
    fseek(file, offset, SEEK_SET);
    fwrite(&blocks_num, FS_BLOCK_NUMBER_SIZE , 1, file);

    /** Writing block size **/
    offset += FS_BLOCK_NUMBER_SIZE;
    fseek(file, offset, SEEK_SET);
    fwrite(&block_size, FS_COUNTER_SIZE, 1, file);

    /** Creating empty descriptors, 100 at the begin **/
    descriptor desc = (descriptor) {.date = "", .name = "", .first = 0, .size=0};
    offset += FS_BLOCK_SIZE_SIZE;
    fseek(file, offset, SEEK_SET);
    for (int i = 0; i < FS_DESCRIPTOR_NUM; i++) {
        fseek(file, offset, SEEK_SET);
        fwrite(&desc, FS_DESCRIPTOR_SIZE, 1, file);
        offset += FS_DESCRIPTOR_SIZE;
    }

    /** Creating empty nodes **/
    node nd = (node) {.next = 0, .is_final  = false};
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

int load_block_number() {
    FILE *file = NULL;
    file = fopen(DISC_NAME, "w");
    if (file == NULL) {
        puts("Unable to reach the disc ...");
        return 0;
    }


}

descriptor **load_descriptors() {
    FILE *file = NULL;
    file = fopen(DISC_NAME, "r");
    if (file == NULL) {
        puts("Unable to reach the disc ...");
        return NULL;
    }
    int offset = FS_FIRST_DESCRIPTOR;
    int disc_size, files_number, block_number, block_size;

    fread(&disc_size, 4, 1, file);

    fseek(file, 4, SEEK_SET);
    fread(&files_number, 4, 1, file);

    fseek(file, 8, SEEK_SET);
    fread(&block_number, 4, 1, file);

    fseek(file, 12, SEEK_SET);
    fread(&block_size, 4, 1, file);

    descriptor **desc = malloc(sizeof(descriptor *) * FS_DESCRIPTOR_NUM);
    for (int i = 0; i < FS_DESCRIPTOR_NUM; i++) {
        desc[i] = malloc(sizeof(descriptor));
        fseek(file, offset + FS_DESCRIPTOR_SIZE * i, SEEK_SET);
        fread(desc[i], FS_DESCRIPTOR_SIZE, 1, file);
        printf("%s %s %d %d\n", desc[i]->name, desc[i]->date, desc[i]->first, desc[i]->size);
    }
    fclose(file);

    return desc;
}

node **load_nodes() {
    FILE *file = NULL;
    file = fopen(DISC_NAME, "r");
    if (file == NULL) {
        puts("Unable to reach the disc ...");
        return NULL;
    }
    int offset = FS_DISC_SIZE + FS_COUNTER_SIZE;
    int block_number;
    fseek(file, offset, SEEK_SET);
    fread(&block_number, FS_BLOCK_NUMBER_SIZE, 1, file);
    offset = FS_FIRST_DESCRIPTOR + FS_DESCRIPTOR_NUM * FS_DESCRIPTOR_SIZE;
    node **nds = malloc(sizeof(node *) * block_number);
    for (int i = 0; i < block_number; i++) {
        nds[i] = malloc(sizeof(node));
        fseek(file, offset + FS_NODE_SIZE * i, SEEK_SET);
        fread(nds[i], FS_NODE_SIZE, 1, file);
        printf("%u %s\n", nds[i]->next, nds[i]->is_final ? "true" : "false");
    }
    fclose(file);
    return nds;
}


void create_file(const char *fname, int bytes) {
    FILE *file = NULL;
    file = fopen(DISC_NAME, "wb");
    if (file == NULL) {
        puts("Couldn't open the disc ...");
        return;
    }
    fclose(file);
    remove_file(fname);



    /* TODO Check if file exists
     * TODO If yes then purge the file and create a new empty one
     */
}

void remove_all_files() {
    FILE *file = NULL;
    file = fopen(DISC_NAME, "r");
    if (file == NULL) {
        puts("Couldn't open the disc ...");
        return;
    }

    int disc_size, block_size;
    fread(&disc_size, FS_DISC_SIZE, 1, file);
    fseek(file, FS_DISC_SIZE + FS_COUNTER_SIZE + FS_BLOCK_NUMBER_SIZE, SEEK_SET);
    fread(&block_size, FS_BLOCK_SIZE_SIZE, 1, file);

    fclose(file);
    remove(DISC_NAME);
    create_disc(disc_size, block_size);
}


void remove_file(const char *name) {
    FILE *file = NULL;
    file = fopen(DISC_NAME, "wb");
    if (file == NULL) {
        puts("Couldn't open the disc ...");
        return;
    }

    descriptor **desc = load_descriptors();
    for (int i = 0; i < FS_DESCRIPTOR_NUM; i++) {
        if (strcmp(desc[i]->name, name) == 0) puts("wow");
    }

    /* TODO Check if such file exists if not, return
     * TODO If yes then purge file
     * TODO When file is purged then these blocks must be returned to empty blocks
     */
}

void move_file(const char *source, const char *dest) {
    /* TODO Check if file of dest has enough space
     * TODO If yes then rename file
     * TODO If no, make fragmentation of the file and rename it
     */
}

int find(const char *file) {

    return 0;
}

