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

typedef int32_t CSV_PARSER_Bool;

struct CSV_PARSER {
	// public
	size_t columns;
	size_t lines;

	struct {
		char *reason;
		size_t column;
		size_t line;
	} error;

	// internal
	uint8_t *buffer;
	uint8_t *position;
	size_t buffer_length;

	void *allocator_context;
};
typedef struct CSV_PARSER CSV_PARSER;

CSV_PARSER_API void csv_parser_init(CSV_PARSER *parser, void *allocator_context);

CSV_PARSER_API void *csv_parser_malloc(size_t size, void *context);
CSV_PARSER_API void csv_parser_free(void *ptr, void *context);

CSV_PARSER_API uint8_t *csv_parser_duplicate_buffer(CSV_PARSER *parser, uint8_t *buffer, size_t length);

// buffer must be null terminated and the length must not count the null terminator
// must not call csv_parser_release, buffer get modified!
// if you don't want your buffer to get modified, call csv_parser_load_duplicated instead
CSV_PARSER_API CSV_PARSER_Bool csv_parser_load_buffer(CSV_PARSER *parser, uint8_t *buffer, size_t length);

CSV_PARSER_API CSV_PARSER_Bool csv_parser_load_duplicated(CSV_PARSER *parser, uint8_t *buffer, size_t length); // requires call to csv_parser_release

#ifndef CSV_PARSER_NO_STDIO
CSV_PARSER_API CSV_PARSER_Bool csv_parser_load_file(CSV_PARSER *parser, FILE *fp); // requires call to csv_parser_release
CSV_PARSER_API CSV_PARSER_Bool csv_parser_load(CSV_PARSER *parser, const char *file_path); // requires call to csv_parser_release
CSV_PARSER_API void csv_parser_release(CSV_PARSER *parser);
#endif

CSV_PARSER_API uint8_t *csv_parser_next(CSV_PARSER *parser, size_t *length);


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

static int64_t _csv_parser_count_columns(CSV_PARSER *parser) {
	int64_t count = 0;

	uint8_t *end = parser->buffer + parser->buffer_length;

	uint8_t *start = parser->position;

	while (parser->position < end && *parser->position != '\n') {
		switch (*parser->position) {
		case '"': // field inside double quotes
			while (parser->position < end && *parser->position != '"') {
				if (*parser->position == '\n' || *parser->position == '\r') {
					parser->error.reason = "Could not find matching \".";
					parser->error.column = parser->position - start;
					return -1;
				}
				parser->position += 1;
			}
			if (parser->position == end) {
				parser->error.reason = "Could not find matching \". Reached end of file.";
				parser->error.column = parser->position - start;
				return -1;
			}
			break;
		case '\'': // field inside single quotes
			while (parser->position < end && *parser->position != '\'') {
				if (*parser->position == '\n' || *parser->position == '\r') {
					parser->error.reason = "Could not find matching '.";
					parser->error.column = parser->position - start;
					return -1;
				}
				parser->position += 1;
			}
			if (parser->position == end) {
				parser->error.reason = "Could not find matching '. Reached end of file.";
				parser->error.column = parser->position - start;
				return -1;
			}
			break;
		case ',':
			*parser->position = '\0';
			start = parser->position + 1;
			count++;
			break;
		default:
			break;
		}

		parser->position += 1;
	}

	// If we have found some number of elements and '\n' at the end, 
	// the total number of columns needs to be increased by 1
	count += (count != 0);

	return count;
}

static CSV_PARSER_Bool _csv_parser_count_lines_and_columns(CSV_PARSER *parser) {
	CSV_PARSER_ASSERT(parser->buffer && parser->position);

	int64_t columns = _csv_parser_count_columns(parser);
	if (columns == -1) {
		parser->error.line = 1;
		return 0;
	}
	else if (columns == 0) {
		parser->error.line = 1;
		parser->error.reason = "Bad CSV file";
	}

	uint8_t *end = parser->buffer + parser->buffer_length;

	while (parser->position < end && _CSV_PARSER_ISSPACE(*parser->position)) {
		*parser->position = '\0';
		parser->position += 1;
	}

	int64_t next_columns;
	while ((next_columns = _csv_parser_count_columns(parser))) {
		if (next_columns == -1 || columns != next_columns) {
			parser->error.line = parser->lines + 1;
			if (columns != next_columns) {
				parser->error.reason = "Not enough number of values.";
			}
			return 0;
		}

		parser->lines += 1;

		while (parser->position < end && _CSV_PARSER_ISSPACE(*parser->position)) {
			*parser->position = '\0';
			parser->position += 1;
		}
	}

	parser->lines += 1;
	parser->columns = columns;
	parser->position = parser->buffer;
	return 1;
}

CSV_PARSER_DEFN_API void csv_parser_init(CSV_PARSER *parser, void *allocator_context) {
	parser->buffer = NULL;
	parser->buffer_length = 0;
	parser->columns = 0;
	parser->lines = 0;
	parser->position = 0;
	parser->allocator_context = allocator_context;

	parser->error.reason = NULL;
	parser->error.column = 0;
	parser->error.line = 0;
}

CSV_PARSER_DEFN_API void *csv_parser_malloc(size_t size, void *context) {
	return CSV_PARSER_MALLOC(size, context);
}

CSV_PARSER_DEFN_API void csv_parser_free(void *ptr, void *context) {
	CSV_PARSER_FREE(ptr, context);
}

CSV_PARSER_DEFN_API uint8_t *csv_parser_duplicate_buffer(CSV_PARSER *parser, uint8_t *buffer, size_t length) {
	uint8_t *dst = csv_parser_malloc((length + 1) * sizeof(*buffer), parser->allocator_context);
	if (dst) {
		CSV_PARSER_MEMCPY(dst, buffer, length);
		dst[length] = 0;
		return dst;
	}
	return NULL;
}

CSV_PARSER_DEFN_API CSV_PARSER_Bool csv_parser_load_buffer(CSV_PARSER *parser, uint8_t *buffer, size_t length) {
	parser->buffer = buffer;
	parser->buffer_length = length;
	parser->position = buffer;
	parser->columns = 0;
	parser->lines = 0;

	return _csv_parser_count_lines_and_columns(parser);
}

CSV_PARSER_DEFN_API CSV_PARSER_Bool csv_parser_load_duplicated(CSV_PARSER *parser, uint8_t *buffer, size_t length) {
	uint8_t *duplicate = csv_parser_duplicate_buffer(parser, buffer, length);
	if (duplicate) {
		return csv_parser_load_buffer(parser, duplicate, length);
	}
	return 0;
}

#ifndef CSV_PARSER_NO_STDIO
CSV_PARSER_DEFN_API CSV_PARSER_Bool csv_parser_load_file(CSV_PARSER *parser, FILE *fp) {
	CSV_PARSER_ASSERT(fp);
	size_t buffer_length = _csv_parser_get_file_size(fp);
	uint8_t *buffer = csv_parser_malloc((buffer_length + 1) * sizeof(*buffer), parser->allocator_context);
	if (buffer == NULL) {
		parser->error.reason = "Allocation failed. Out of memory.";
		return 0;
	}
	buffer[buffer_length] = 0;
	size_t result = fread(buffer, buffer_length, 1, fp);

	if (result != 1) {
		parser->error.reason = "File could not be read.";
		return 0;
	}
	return csv_parser_load_buffer(parser, buffer, buffer_length);
}

CSV_PARSER_DEFN_API CSV_PARSER_Bool csv_parser_load(CSV_PARSER *parser, const char *file_path) {
	FILE *fp = fopen(file_path, "rb");
	if (fp) {
		CSV_PARSER_Bool result = csv_parser_load_file(parser, fp);
		fclose(fp);
		return result;
	}
	parser->error.reason = "File could not be opened for reading.";
	return 0;
}

CSV_PARSER_DEFN_API void csv_parser_release(CSV_PARSER *parser) {
	csv_parser_free(parser->buffer, parser->allocator_context);
}

#endif

CSV_PARSER_DEFN_API uint8_t *csv_parser_next(CSV_PARSER *parser, size_t *length) {
	uint8_t *end = parser->buffer + parser->buffer_length;

	while (parser->position < end && !*parser->position)
		parser->position += 1;

	uint8_t *next_token = parser->position;

	while (parser->position < end && *parser->position)
		parser->position += 1;

	*length = parser->position - next_token;

	return next_token;
}

#endif // CSV_PARSER_IMPLEMENTATION

#endif
