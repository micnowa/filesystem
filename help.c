#include "help.h"

/**
// Created by Michał Nowaliński on 8/1/20.
**/

void mkfs_help() {
    puts("This command creates a new disc with given file size");
    puts("Options:");
    puts("size - Number of bytes that filesystem will have");
    puts("block - size of single block");
}

void touch_help() {
    puts("This command creates a new -n file with filename given and -b size of bytes");
    puts("If a file with given name already exists it is overwritten and its contents are gone");
    puts("Options:");
    puts("-n - File name");
    puts("-b - Number of bytes to write");
}

void mv_help() {
    puts("Moves -s file to -d file, if file -d doesn't exist then it is created");
    puts("Content of file -s becomes content of file fy");
    puts("Options:");
    puts("-s - Name of source file");
    puts("-d - Name of destination file");
}

void get_help() {
    puts("Prints file of given name into standard output");
    puts("If the file does not exists it puts nothing");
}

void cp_help() {
    puts("Copies -s file to -d file, if file -d doesn't exist then it is created");
    puts("Content of file -s becomes content of file fy");
    puts("Options:");
    puts("-s - Name of source file");
    puts("-d - Name of destination file");
}

void extcp_help() {
    puts("Copies -s file from EXTERNAL SOURCE to -d file, if file -d doesn't exist then it is created");
    puts("Content of file -s becomes content of file fy");
    puts("Options:");
    puts("-s - Name of source file");
    puts("-d - Name of destination file");
}


void rm_help() {
    puts("Removes file -n from the disc");
    puts("Options:");
    puts("-n - File name");
}

void ls_help() {
    puts("Lists all files in the disc");
}

void rmdisc_help() {
    puts("Removes disc completely");
}

void info_help() {
    puts("Prints disc's info");
    puts("Size");
    puts("File counter");
    puts("Number of available blocks");
    puts("Size of single block");
}