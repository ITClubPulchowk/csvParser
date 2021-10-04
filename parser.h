#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

void parse_file(const char *path);

// temp use
int get_column_index_from_header_name(const char *header_name);

// printing functions
void print_csv();
void print_record(int row, int column);
void print_row(int row);
void print_column(int column);

#endif
