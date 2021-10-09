#ifndef PARSER_H
#define PARSER_H

#include "base.h"

// TODO: arrange elements as per the underlying padding
struct csv_parser {
    int col_count; // initialize to 1
    int line_count; // initialize to 1
    int total_records; // initialize to 0
    int* field_offset_array; // initialize to NULL
    char* file_buf; // initialize to NULL
};

// run this function before everything else
void load_file_buffer(const char *file_path, struct csv_parser* parser);

// access columns by header name
int get_column_index_from_header_name(struct csv_parser* parser, const char *header_name);

// printing functions
int print_csv(struct csv_parser* parser);
int print_record(struct csv_parser* parser, int row, int column);
int print_row(struct csv_parser* parser, int row);
int print_column(struct csv_parser* parser, int column);

// free buffer
void free_csv_resources(struct csv_parser* parser);

#endif
