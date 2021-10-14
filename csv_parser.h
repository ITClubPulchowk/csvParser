#ifndef CSV_PARSER_H
#define CSV_PARSER_H

//
// [CONFIGURATION]
//

#ifndef CSV_PARSER_API
#ifndef CSV_PARSER_API_STATIC
#define CSV_PARSER_API extern
#define CSV_PARSER_DEFN_API 
#else
#define CSV_PARSER_API static
#define CSV_PARSER_DEFN_API static
#endif
#else
#ifndef CSV_PARSER_DEFN_API 
#error "If CSV_PARSER_API is defined, then CSV_PARSER_DEFN_API must also be defined"
#endif
#endif

#ifndef CSV_PARSER_ASSERT
#include <assert.h>
#define CSV_PARSER_ASSERT assert
#endif

#ifndef CSV_PARSER_MALLOC
#include <stdlib.h>
#define CSV_PARSER_MALLOC(size, context) malloc(size)
#define CSV_PARSER_FREE(ptr, context) free(ptr)
#else
#ifndef CSV_PARSER_FREE
#error "If CSV_PARSER_MALLOC is defined, then CSV_PARSER_FREE must also be defined"
#endif
#endif

#ifndef CSV_PARSER_MEMCPY
#include <string.h>
#define CSV_PARSER_MEMCPY memcpy
#endif

#ifndef CSV_PARSER_NO_STDIO
#include <stdio.h>
#endif

#include <stdint.h>


//
// [DECLARATIONS]
//

typedef int32_t csv_parser_bool;

struct csv_parser {
	// public
	size_t columns;
	size_t lines;

	// internal
	uint8_t *buffer;
	size_t position;
	size_t buffer_length;

	void *allocator_context;
};
typedef struct csv_parser csv_parser;

CSV_PARSER_API void csv_parser_init(csv_parser *parser, void *allocator_context);

CSV_PARSER_API void *csv_parser_malloc(size_t size, void *context);
CSV_PARSER_API void csv_parser_free(void *ptr, void *context);

CSV_PARSER_API uint8_t *csv_parser_duplicate_buffer(csv_parser *parser, uint8_t *buffer, size_t length);

// buffer must be null terminated and the length must not count the null terminator
// must not call csv_parser_release, buffer get modified!
// if you don't want your buffer to get modified, call csv_parser_load_duplicated instead
CSV_PARSER_API csv_parser_bool csv_parser_load_buffer(csv_parser *parser, uint8_t *buffer, size_t length);

CSV_PARSER_API csv_parser_bool csv_parser_load_duplicated(csv_parser *parser, uint8_t *buffer, size_t length); // requires call to csv_parser_release

#ifndef CSV_PARSER_NO_STDIO
CSV_PARSER_API csv_parser_bool csv_parser_load_file(csv_parser *parser, FILE *fp); // requires call to csv_parser_release
CSV_PARSER_API csv_parser_bool csv_parser_load(csv_parser *parser, const char *file_path); // requires call to csv_parser_release
CSV_PARSER_API void csv_parser_release(csv_parser *parser);
#endif

CSV_PARSER_API uint8_t *csv_parser_next(csv_parser *parser, size_t *length);


//
// [IMPLEMENTATION]
//

#ifdef CSV_PARSER_IMPLEMENTATION

static size_t _csv_parser_get_file_size(FILE *fp) {
	fseek(fp, 0L, SEEK_END);
	long f_size = ftell(fp);
	fseek(fp, 0L, SEEK_SET);
	return f_size;
}

#define _CSV_PARSER_ISSPACE(ch) ((ch) == ' ' || (ch) == '\f' || (ch) == '\n' || (ch) == '\r' || (ch) == '\t' || (ch) == '\v')

static int64_t _csv_parser_count_columns(csv_parser *parser) {
	int64_t count = 0;
	size_t i = parser->position;

	while (i < parser->buffer_length && parser->buffer[parser->buffer_length] != '\n') {
		switch (parser->buffer[i]) {
		case '"': // field inside double quotes
			while (parser->buffer[i] != '"') {
				if (parser->buffer[i] == '\n' || parser->buffer[i] == '\r')
					return -1;
				i += 1;
			}
			break;
		case '\'': // field inside single quotes
			while (parser->buffer[i] != '\'') {
				if (parser->buffer[i] == '\n' || parser->buffer[i] == '\r')
					return -1;
				i += 1;
			}
			break;
		case ',':
			parser->buffer[i] = '\0';
			count++;
			break;
		default:
			break;
		}

		i++;
	}

	// If we have found some number of elements and '\n' at the end, 
	// the total number of columns needs to be increased by 1
	count += (count != 0);

	parser->position = i + 1;

	return count;
}

static csv_parser_bool _csv_parser_count_lines_and_columns(csv_parser *parser) {
	CSV_PARSER_ASSERT(parser->buffer);
	int64_t columns = _csv_parser_count_columns(parser);
	if (columns == -1) return 0;

	int64_t next_columns;
	while ((next_columns = _csv_parser_count_columns(parser))) {
		if (next_columns == -1 || (columns != next_columns && columns != 0))
			return 0;

		while (_CSV_PARSER_ISSPACE(parser->buffer[parser->position]))
			parser->position += 1;

		parser->lines += (columns != 0);
	}

	parser->columns = columns;
	parser->position = 0;
	return 1;
}

CSV_PARSER_DEFN_API void csv_parser_init(csv_parser *parser, void *allocator_context) {
	parser->buffer = NULL;
	parser->buffer_length = 0;
	parser->columns = 0;
	parser->lines = 0;
	parser->position = 0;
	parser->allocator_context = allocator_context;
}

CSV_PARSER_DEFN_API void *csv_parser_malloc(size_t size, void *context) {
	return CSV_PARSER_MALLOC(size, context);
}

CSV_PARSER_DEFN_API void csv_parser_free(void *ptr, void *context) {
	CSV_PARSER_FREE(ptr, context);
}

CSV_PARSER_DEFN_API uint8_t *csv_parser_duplicate_buffer(csv_parser *parser, uint8_t *buffer, size_t length) {
	uint8_t *dst = csv_parser_malloc((length + 1) * sizeof(*buffer), parser->allocator_context);
	if (dst) {
		CSV_PARSER_MEMCPY(dst, buffer, length);
		dst[length] = 0;
		return dst;
	}
	return NULL;
}

CSV_PARSER_DEFN_API csv_parser_bool csv_parser_load_buffer(csv_parser *parser, uint8_t *buffer, size_t length) {
	parser->buffer = buffer;
	parser->buffer_length = length;
	parser->position = 0;
	parser->columns = 0;
	parser->lines = 0;

	return _csv_parser_count_lines_and_columns(parser);
}

CSV_PARSER_DEFN_API csv_parser_bool csv_parser_load_duplicated(csv_parser *parser, uint8_t *buffer, size_t length) {
	uint8_t *duplicate = csv_parser_duplicate_buffer(parser, buffer, length);
	if (duplicate) {
		return csv_parser_load_buffer(parser, duplicate, length);
	}
	return 0;
}

#ifndef CSV_PARSER_NO_STDIO
CSV_PARSER_DEFN_API csv_parser_bool csv_parser_load_file(csv_parser *parser, FILE *fp) {
	CSV_PARSER_ASSERT(fp);
	size_t buffer_length = _csv_parser_get_file_size(fp);
	uint8_t *buffer = csv_parser_malloc((buffer_length + 1) * sizeof(*buffer), parser->allocator_context);
	if (buffer == NULL)
		return 0;
	buffer[buffer_length] = 0;
	size_t result = fread(buffer, buffer_length, 1, fp);

	if (result != 1) return 0;
	return csv_parser_load_buffer(parser, buffer, buffer_length);
}

CSV_PARSER_DEFN_API csv_parser_bool csv_parser_load(csv_parser *parser, const char *file_path) {
	FILE *fp = fopen(file_path, "rb");
	if (fp) {
		csv_parser_bool result = csv_parser_load_file(parser, fp);
		fclose(fp);
		return result;
	}
	return 0;
}

CSV_PARSER_DEFN_API void csv_parser_release(csv_parser *parser) {
	csv_parser_free(parser->buffer, parser->allocator_context);
}

#endif

CSV_PARSER_DEFN_API uint8_t *csv_parser_next(csv_parser *parser, size_t *length) {
	while (!parser->buffer[parser->position]) {
		parser->position += 1;
	}

	uint8_t *next_token = parser->buffer + parser->position;

	while (parser->buffer[parser->position]) {
		parser->position += 1;
	}

	*length = (parser->buffer + parser->position - 1) - next_token;

	return next_token;
}

#endif // CSV_PARSER_IMPLEMENTATION

#endif
