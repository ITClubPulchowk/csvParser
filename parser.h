#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>

void parse_file(const char *path);

// printing functions
void print_csv();
void print_record(int row, int column);
void print_row(int row);
void print_column(int column);

#endif
