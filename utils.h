//
// Created by root on 7/1/20.
//

#ifndef UTILS_H
#define UTILS_H

#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

FILE* open_disc(const char *mode);

bool is_positive_int(char *size);

bool valid_fname(const char *name);

char* extract_name(const char *name);

bool str_with_quotes(const char *str);

char* current_date();

int calculate_block_number(int disc_size, int block_size);

int node_offset(int number);

int block_offset(int number, int block_number, int block_size);

int load_disc_size();

int load_file_counter();

char* read_file_Bytes(const char *name);

int fsize(FILE *fp);

int load_block_number();

int load_block_size();

bool enough_nodes(int bytes);

void increment_counter(bool plus);

#endif //UTILS_H
