#ifndef CSV_PARSER_H
#define CSV_PARSER_H

#ifndef CSV_PARSER_ASSERT
#include <assert.h>
#define CSV_PARSER_ASSERT assert
#endif

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef int32_t csv_parser_bool;

struct csv_parser {
	// public
	size_t columns;
	size_t lines;

	// internal
	uint8_t *buffer;
	size_t parser_pos;
	size_t buffer_length;

	void *allocator_context;
};
typedef struct csv_parser csv_parser;

void csv_parser_init(csv_parser *parser, void *allocator_context);

void *csv_parser_malloc(size_t size, void *context);
void csv_parser_free(void *ptr, void *context);

void csv_parser_load_buffer(csv_parser *parser, uint8_t *buffer, size_t length); // buffer must be null terminated
csv_parser_bool csv_parser_load_file(csv_parser *parser, FILE *fp);
csv_parser_bool csv_parser_load(csv_parser *parser, const char *file_path);
void csv_parser_release(csv_parser *parser);

uint8_t *csv_parser_next(csv_parser *parser);

#endif
