#ifndef CSV_PARSER_H
#define CSV_PARSER_H

#include "base.h"
#include <assert.h>
#include <stdint.h>

struct csv_parser {
	// public
	size_t columns;
	size_t lines;

	// internal
	uint8_t *buffer;
	size_t parser_pos;
	size_t buffer_length;
};
typedef struct csv_parser csv_parser;

void csv_parser_load(csv_parser *parser, const char *file_path);
void csv_parser_free(csv_parser *parser);

char *csv_parser_next(csv_parser *parser);

#endif
