/**
// Created by Michał Nowaliński on 7/1/20.
**/
#include "file_system.h"


void read_disc_info() {
    FILE *file = open_disc("r+");
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

    fclose(file);
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

descriptor **load_descriptors_() {
    FILE *file = open_disc("r+");
    int offset = FS_FIRST_DESCRIPTOR;
    descriptor **desc = malloc(sizeof(descriptor *) * FS_DESCRIPTOR_NUM);
    for (int i = 0; i < FS_DESCRIPTOR_NUM; i++) {
        desc[i] = malloc(sizeof(descriptor));
        fseek(file, offset, SEEK_SET);
        fread(desc[i], FS_DESCRIPTOR_SIZE, 1, file);
        offset += FS_DESCRIPTOR_SIZE;
    }
    fclose(file);

    return desc;
}

node **load_nodes_() {
    FILE *file = open_disc("r+");
    int offset = FS_DISC_SIZE_SIZE + FS_COUNTER_SIZE;
    int block_number;
    fseek(file, offset, SEEK_SET);
    fread(&block_number, FS_BLOCK_NUMBER_SIZE, 1, file);
    offset = FS_FIRST_NODE;
    node **nds = malloc(sizeof(node *) * block_number);
    for (int i = 0; i < block_number; i++) {
        nds[i] = malloc(sizeof(node));
        fseek(file, offset, SEEK_SET);
        fread(nds[i], FS_NODE_SIZE, 1, file);
        offset += FS_NODE_SIZE;
    }
    fclose(file);
    return nds;
}

void write_descriptor_(descriptor desc, int offset) {
    FILE *file = open_disc("r+");
    fseek(file, offset, SEEK_SET);
    fwrite(&desc, FS_DESCRIPTOR_SIZE, 1, file);
    fclose(file);
}

void create_file(const char *fname, int bytes) {
    if (!enough_nodes(bytes)) {
        puts("Not enough space ...");
        return;
    }
//    if (find(fname) != NULL) remove_file(fname);

    int descriptor_position = -1;
    descriptor **desc = load_descriptors_();
    node **nd = load_nodes_();
    int block_size = load_block_size();
    int block_number = load_block_number();
    int blocks_to_write, last_block_size;
    if (!(bytes % block_size)) {
        blocks_to_write = bytes / block_size;
        last_block_size = block_size;
    } else {
        blocks_to_write = bytes / block_size + 1;
        last_block_size = bytes - (blocks_to_write - 1) * block_size;
    }

    /** Looking for descriptor **/
    for (int i = 0; i < FS_DESCRIPTOR_NUM; i++)
        if (desc[i]->first == -1) {
            descriptor_position = i;
            break;
        }
    if (descriptor_position == -1) {
        puts("Exceeded files limit ...");
        return;
    }

    int current = 0;
    int desc_offset = FS_FIRST_DESCRIPTOR;
    desc_offset += FS_DESCRIPTOR_SIZE * descriptor_position;
    while (nd[current]->has_data) {
        current++;
    }
    strcpy(desc[descriptor_position]->date, current_date());
    strcpy(desc[descriptor_position]->name, fname);
    desc[descriptor_position]->size = bytes;
    desc[descriptor_position]->first = current;

    /** Incrementing file number **/
    increment_counter(true);

    /** Writing descriptor **/
    write_descriptor_(*desc[descriptor_position], desc_offset);


    int nd_off, bl_off;
    void *block;
    int next_element = current;
    FILE *file = open_disc("r+");
    while (blocks_to_write != 0) {
        /** Writing node **/
        current = next_element;
        nd[current]->has_data = true;
        if (blocks_to_write == 1) {
            next_element = -1;
        } else {
            while (nd[next_element]->has_data) {
                next_element++;
            }
        }
        nd[current]->next = next_element;

        nd_off = node_offset(current);
        fseek(file, nd_off, SEEK_SET);
        fwrite(nd[current], FS_NODE_SIZE, 1, file);

        /** Writing block **/

        bl_off = block_offset(current, block_number, block_size);
        fseek(file, bl_off, SEEK_SET);
        if (blocks_to_write == 1) {
            block = calloc(1, last_block_size);
            fseek(file, bl_off, SEEK_SET);
            fwrite(block, last_block_size, 1, file);
        } else {
            block = calloc(1, block_size);
            fseek(file, bl_off, SEEK_SET);
            fwrite(block, block_size, 1, file);
        }
        free(block);
        blocks_to_write--;
    }
    fclose(file);
}

void write_descriptors(descriptor **desc) {
    FILE *file = open_disc("r+");
    int offset = FS_FIRST_DESCRIPTOR;
    for (int i = 0; i < FS_DESCRIPTOR_NUM; i++) {
        fseek(file, offset, SEEK_SET);
        fwrite(desc[i], FS_NODE_SIZE, 1, file);
        offset += FS_DESCRIPTOR_SIZE;
    }
    fclose(file);
}

void write_nodes(node **nd, int num) {
    FILE *file = open_disc("r+");
    int offset = FS_FIRST_NODE;
    for (int i = 0; i < num; i++) {
        fseek(file, offset, SEEK_SET);
        node *nds = nd[i];
        fwrite(nds, FS_NODE_SIZE, 1, file);
        offset += FS_NODE_SIZE;
    }
    fclose(file);
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
    int num;
    descriptor *desc = find_and_return_number(name, &num);
    if (desc == NULL) return;
    int bytes = desc->size;

    /** Decrementing files counter **/
    increment_counter(false);

    int block_size = load_block_size();
    int total_block_number = load_block_number();
    int blocks_number = !(bytes % block_size) ? bytes / block_size : bytes / block_size + 1;
    int current_node = desc->first, next;
    int offset;
    descriptor empty_desc = (descriptor) {.date = "", .name = "", .first = -1, .size=0};
    node empty_node = (node) {.next = -1, .has_data  = false};
    void *empty_block = malloc(block_size);
    FILE *file = open_disc("r+");

    /** Writing empty descriptor **/
    offset = FS_FIRST_DESCRIPTOR + FS_DESCRIPTOR_SIZE * num;
    fseek(file, offset, SEEK_SET);
    fwrite(&empty_desc, FS_DESCRIPTOR_SIZE, 1, file);

    /** Writing empty nodes and blocks **/
    for (int i = 0; i < blocks_number; i++) {
        offset = node_offset(current_node);
        fseek(file, offset, SEEK_SET);
        fread(&next, sizeof(int), 1, file);
        fseek(file, offset, SEEK_SET);
        fwrite(&empty_node, FS_NODE_SIZE, 1, file);
        offset = block_offset(current_node, total_block_number, block_size);
        fseek(file, offset, SEEK_SET);
        fwrite(&empty_block, block_size, 1, file);
        current_node = next;
    }
    fclose(file);

}

void move_file(const char *source, const char *dest) {}

descriptor *find(const char *file) {
    descriptor **desc = load_descriptors_();
    for (int i = 0; i < FS_DESCRIPTOR_NUM; i++) {
        if (strcmp(desc[i]->name, file) == 0) {
            return desc[i];
        }
    }
    return NULL;
}

descriptor *find_and_return_number(const char *file, int *num) {
    descriptor **desc = load_descriptors_();
    for (int i = 0; i < FS_DESCRIPTOR_NUM; i++) {
        if (strcmp(desc[i]->name, file) == 0) {
            *num = i;
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
