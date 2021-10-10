#ifndef PARSER_H
#define PARSER_H

#include "base.h"

// TODO: arrange elements as per the underlying padding
struct csv_parser {
    int col_count; // initialize to 1
    int line_count; // initialize to 1
    int total_records; // initialize to 0
    int parser_pos; // initialize to -1
    int* field_offset_array; // initialize to NULL
    char* file_buf; // initialize to NULL
};
typedef struct csv_parser csv_parser;
extern csv_parser csv_defaults;

// run this function before everything else
void load_file_buffer(csv_parser* parser, const char *file_path);

// sequential parsing
char* parse_next(csv_parser* parser);

// access columns by header name
int get_column_index_from_header_name(csv_parser* parser, const char *header_name);

// printing functions
int print_csv(csv_parser* parser);
int print_record(csv_parser* parser, int row, int column);
int print_row(csv_parser* parser, int row);
int print_column(csv_parser* parser, int column);

// free buffer
void free_csv_resources(csv_parser* parser);

#endif
