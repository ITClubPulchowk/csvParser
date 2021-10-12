#ifndef CSV_PARSER_H
#define CSV_PARSER_H

#include "base.h"

struct csv_parser {
	// public
	int columns;
	int lines;

	// internal
	int parser_pos;
	int file_len;
	char *file_buf;
};
typedef struct csv_parser csv_parser;

void csv_parser_load(csv_parser *parser, const char *file_path);
void csv_parser_free(csv_parser *parser);

char *csv_parser_next(csv_parser *parser);

#endif
