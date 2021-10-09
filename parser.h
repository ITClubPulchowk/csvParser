#ifndef PARSER_H
#define PARSER_H

#include "base.h"

// run this function before everything else
void parse_file(const char *path);

// access columns by header name
int get_column_index_from_header_name(const char *header_name);

// printing functions
int print_csv();
int print_record(int row, int column);
int print_row(int row);
int print_column(int column);

// free buffer
void free_csv_resources();

#endif
