#ifndef CSV_PARSER_H
#define CSV_PARSER_H

#ifndef CSV_PARSER_ASSERT
#include <assert.h>
#define CSV_PARSER_ASSERT assert
#endif

#ifndef CSV_PARSER_MALLOC
#include <stdlib.h>
#define CSV_PARSER_MALLOC(size, context) malloc(size)
#define CSV_PARSER_FREE(ptr, context) free(ptr)
#endif

#ifndef CSV_PARSER_NO_STDIO
#include <stdio.h>
#endif

#include <stdint.h>

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

uint8_t *csv_parser_duplicate_buffer(csv_parser *parser, uint8_t *buffer, size_t length);

// buffer must be null terminated
// must not call csv_parser_release, buffer get modified!
// if you don't want your buffer to get modified, call csv_parser_load_duplicated instead
void csv_parser_load_buffer(csv_parser *parser, uint8_t *buffer, size_t length);

csv_parser_bool csv_parser_load_duplicated(csv_parser *parser, uint8_t *buffer, size_t length); // requires call to csv_parser_release

#ifndef CSV_PARSER_NO_STDIO
csv_parser_bool csv_parser_load_file(csv_parser *parser, FILE *fp); // requires call to csv_parser_release
csv_parser_bool csv_parser_load(csv_parser *parser, const char *file_path); // requires call to csv_parser_release
void csv_parser_release(csv_parser *parser);
#endif

uint8_t *csv_parser_next(csv_parser *parser);

#endif
