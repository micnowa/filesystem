/**
// Created by Michał Nowaliński on 7/1/20.
**/

#ifndef FILE_SYSTEM_H
#define FILE_SYSTEM_H

#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include "utils.h"

/** Disc size **/
#define FS_DISC_SIZE_SIZE sizeof(int)

/** Counter **/
#define FS_COUNTER_SIZE sizeof(int)

/** Block size **/
#define FS_BLOCK_NUMBER_SIZE sizeof(int)

/** Block size **/
#define FS_BLOCK_SIZE_SIZE sizeof(int)

/** Descriptor **/
#define FS_DESCRIPTOR_NUM 100 /** Initially it is space for 100 files' descriptions **/
#define FS_DESCRIPTOR_SIZE sizeof(struct descriptor)
#define FS_NAME_SIZE sizeof(char)*32
#define FS_DATE_SIZE (17*sizeof(char))

/** Node **/
#define FS_NODE_SIZE sizeof(struct node)

/** Special pointers **/
#define FS_FIRST_DESCRIPTOR FS_DISC_SIZE_SIZE+FS_COUNTER_SIZE+FS_BLOCK_NUMBER_SIZE+FS_BLOCK_SIZE_SIZE
#define FS_FIRST_NODE FS_FIRST_DESCRIPTOR+FS_DESCRIPTOR_SIZE*FS_DESCRIPTOR_NUM

/** Disc name **/
#define DISC_NAME "disc"

typedef struct descriptor {
    char name[FS_NAME_SIZE];
    char date[FS_DATE_SIZE];
    int first;
    int size;
} descriptor;

typedef struct node {
    int next;
    bool has_data;
} node;

void read_disc_info();

void create_disc(int disc_size, int block_size);

int *load_nodes_numbers(int *block_number, descriptor desc, int block_size);

descriptor **load_descriptors_();

void write_descriptors(descriptor **desc);

void write_nodes(node **nd, int num);

node **load_nodes_();

void write_descriptor_(descriptor desc, int offset);

void create_file(const char *fname, int bytes);

void remove_all_files();

void remove_file(const char *name);

void move_file(const char *source, const char *dest);

descriptor *find(const char *file);

#endif //FILE_SYSTEM_H