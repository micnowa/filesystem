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
#define FS_DISC_SIZE sizeof(int)

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
#define FS_FIRST_DESCRIPTOR FS_DISC_SIZE+FS_COUNTER_SIZE+FS_BLOCK_NUMBER_SIZE+FS_BLOCK_SIZE_SIZE
#define FS_FIRST_NODE  FS_DISC_SIZE+FS_COUNTER_SIZE+FS_BLOCK_NUMBER_SIZE+FS_BLOCK_SIZE_SIZE+FS_DESCRIPTOR_SIZE*FS_DESCRIPTOR_NUM

/** Disc name **/
#define DISC_NAME "disc"

int NODE_NUMBER;
int DISC_SIZE;
int BLOCK_SIZE;

typedef struct descriptor {
    char name[FS_NAME_SIZE];
    char date[FS_DATE_SIZE];
    int first;
    int size;
} descriptor;

typedef struct node {
    int next;
    bool is_final;
} node;

void create_disc(int disc_size, int block_size);

int load_block_number();

descriptor **load_descriptors();

node **load_nodes();

void create_file(const char *fname, int bytes);

void remove_all_files();

void remove_file(const char *name);

void move_file(const char *source, const char *dest);

int find(const char *file);

#endif //FILE_SYSTEM_H