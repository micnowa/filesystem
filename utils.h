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

bool is_positive_int(char *size);

bool valid_fname(const char *name);

char* extract_name(const char *name);

char* extract_value(const char *arg, const char *phrase);

bool str_with_quotes(const char *str);

char* current_date();

int calculate_block_number(int disc_size, int block_size);

#endif //UTILS_H
