#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

// run this function before everything else
void parse_file(const char *path);

// access columns by header name
int get_column_index_from_header_name(const char *header_name);

// printing functions
void print_csv();
void print_record(int row, int column);
void print_row(int row);
void print_column(int column);

void free_csv_resources();

#endif
