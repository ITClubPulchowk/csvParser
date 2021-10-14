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

// buffer must be null terminated
// must not call csv_parser_release, buffer get modified!
// if you don't want your buffer to get modified, call csv_parser_load_duplicated instead
CSV_PARSER_API void csv_parser_load_buffer(csv_parser *parser, uint8_t *buffer, size_t length);

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

static size_t _csv_parser_calculate_number_of_columns(csv_parser *parser) {
	CSV_PARSER_ASSERT(parser->buffer);

	size_t count = 1;
	size_t i = 0;
	while (parser->buffer[i] != '\n') {
		switch (parser->buffer[i]) {
		case '"': // field inside double quotes
			while (parser->buffer[++i] != '"');
			break;
		case '\'': // field inside single quotes
			while (parser->buffer[++i] != '\'');
			break;
		case ',':
			count++;
			break;
		default:
			break;
		}
		i++;
	}
	return count;
}

static size_t _csv_parser_calculate_number_of_lines(uint8_t *buffer, size_t len) {
	size_t lines = 0;
	for (size_t i = 0; i < len; i++) {
		switch (buffer[i]) {
		case '\n':
			lines++;
			break;

		case '\r':
			if (i + 1 < len && buffer[i + 1] == '\n') {
				lines++;
				i++;
			}
			break;

		default:
			break;
		}
	}
	// If the file doesn't end with a newline, we need to add once,
	// since that line won't be counted
	return lines + (buffer[len - 1] != '\n' || buffer[len - 1] != '\n');
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

CSV_PARSER_DEFN_API void csv_parser_load_buffer(csv_parser *parser, uint8_t *buffer, size_t length) {
	parser->buffer = buffer;
	parser->buffer_length = length;

	// set csv properties
	parser->position = 0;
	parser->columns = _csv_parser_calculate_number_of_columns(parser);
	parser->lines = _csv_parser_calculate_number_of_lines(parser->buffer, parser->buffer_length);
}

CSV_PARSER_DEFN_API csv_parser_bool csv_parser_load_duplicated(csv_parser *parser, uint8_t *buffer, size_t length) {
	uint8_t *duplicate = csv_parser_duplicate_buffer(parser, buffer, length);
	if (duplicate) {
		csv_parser_load_buffer(parser, duplicate, length);
		return 1;
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
	csv_parser_load_buffer(parser, buffer, buffer_length);

	return 1;
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
	uint8_t *next_token = parser->buffer + parser->position;
	size_t parse_status = 1;
	for (size_t i = parser->position; parse_status && i < parser->buffer_length; i++) {
		switch (parser->buffer[i]) {
		case '"':
			while (parser->buffer[++i] != '"');
			break;

		case '\'':
			while (parser->buffer[++i] != '\'');
			break;

		case '\n':
			parser->buffer[i] = '\0';
			parser->position = ++i;
			parse_status = 0;
			break;

		case '\r':
			if (i + 1 < parser->buffer_length && parser->buffer[i + 1] == '\n') {
				i++;
				parser->buffer[i] = '\0';
				parser->position = ++i;
				parse_status = 0;
			}
			break;

		case ',':
			parser->buffer[i] = '\0';
			parser->position = ++i;
			parse_status = 0;
			break;

		default:
			break;
		}
	}

	*length = (parser->buffer + parser->position - 1) - next_token;

	return next_token;
}

#endif // CSV_PARSER_IMPLEMENTATION

#endif
