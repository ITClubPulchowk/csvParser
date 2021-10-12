#include "csv_parser.h"

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
	size_t lines = 1;
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
	return lines;
}

void csv_parser_init(csv_parser *parser, void *allocator_context) {
	parser->buffer = NULL;
	parser->buffer_length = 0;
	parser->columns = 0;
	parser->lines = 0;
	parser->parser_pos = 0;
	parser->allocator_context = allocator_context;
}

void *csv_parser_malloc(size_t size, void *context) {
	return CSV_PARSER_MALLOC(size, context);
}

void csv_parser_free(void *ptr, void *context) {
	CSV_PARSER_FREE(ptr, context);
}

uint8_t *csv_parser_duplicate_buffer(csv_parser *parser, uint8_t *buffer, size_t length) {
	uint8_t *dst = csv_parser_malloc((length + 1) * sizeof(*buffer), parser->allocator_context);
	if (dst) {
		memcpy(dst, buffer, length);
		dst[length] = 0;
		return dst;
	}
	return NULL;
}

void csv_parser_load_buffer(csv_parser *parser, uint8_t *buffer, size_t length) {
	parser->buffer = buffer;
	parser->buffer_length = length;

	// set csv properties
	parser->parser_pos = 0;
	parser->columns = _csv_parser_calculate_number_of_columns(parser);
	parser->lines = _csv_parser_calculate_number_of_lines(parser->buffer, parser->buffer_length);
}

csv_parser_bool csv_parser_load_duplicated(csv_parser *parser, uint8_t *buffer, size_t length) {
	uint8_t *duplicate = csv_parser_duplicate_buffer(parser, buffer, length);
	if (duplicate) {
		csv_parser_load_buffer(parser, duplicate, length);
		return 1;
	}
	return 0;
}

#ifndef CSV_PARSER_NO_STDIO
csv_parser_bool csv_parser_load_file(csv_parser *parser, FILE *fp) {
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

csv_parser_bool csv_parser_load(csv_parser *parser, const char *file_path) {
	FILE *fp = fopen(file_path, "rb");
	if (fp) {
		csv_parser_bool result = csv_parser_load_file(parser, fp);
		fclose(fp);
		return result;
	}
	return 0;
}

void csv_parser_release(csv_parser *parser) {
	csv_parser_free(parser->buffer, parser->allocator_context);
}

#endif

uint8_t *csv_parser_next(csv_parser *parser) {
	uint8_t *next_token = parser->buffer + parser->parser_pos;
	size_t parse_status = 1;
	for (size_t i = parser->parser_pos; parse_status && i < parser->buffer_length; i++) {
		switch (parser->buffer[i]) {
		case '"':
			while (parser->buffer[++i] != '"');
			break;

		case '\'':
			while (parser->buffer[++i] != '\'');
			break;

		case '\n':
			parser->buffer[i] = '\0';
			parser->parser_pos = ++i;
			parse_status = 0;
			break;

		case '\r':
			if (i + 1 < parser->buffer_length && parser->buffer[i + 1] == '\n') {
				i++;
				parser->buffer[i] = '\0';
				parser->parser_pos = ++i;
				parse_status = 0;
			}
			break;

		case ',':
			parser->buffer[i] = '\0';
			parser->parser_pos = ++i;
			parse_status = 0;
			break;

		default:
			break;
		}
	}
	return next_token;
}
