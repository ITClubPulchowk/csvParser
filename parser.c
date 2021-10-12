#include "parser.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

const char *gerror_message(int error_code) {
	switch (error_code) {
	case -1:
		return "Function call before parsing csv";
	case -2:
		return "Function call before reading file";
	case -3:
		return "Function parameter is invalid";
	default:
		return "";
	}
}

static int csv_parser_get_file_size(FILE *fp) {
	fseek(fp, 0L, SEEK_END);
	int f_size = ftell(fp);
	fseek(fp, 0L, SEEK_SET);
	return f_size;
}

static int csv_parser_calculate_number_of_columns(csv_parser *parser) {
	assert(parser->buffer);

	int count = 1; // maybe use 0 for default value?
	int i = 0;
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

// TODO: get this threaded and running
static int csv_parser_calculate_number_of_lines(char *buffer, int len) {
	int lines = 1;
	for (int i = 0; i < len; i++) {
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

char *csv_parser_next(csv_parser *parser) {
	char *next_token = parser->buffer + parser->parser_pos;
	int parse_status = 1;
	for (int i = parser->parser_pos; parse_status && i < parser->buffer_length; i++) {
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

			// TODO: implement for \r\n, test on windows
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

void csv_parser_load(csv_parser *parser, const char *file_path) {
	FILE *fp = fopen(file_path, "rb");

	parser->buffer_length = csv_parser_get_file_size(fp);
	parser->buffer = malloc(parser->buffer_length * sizeof(*parser->buffer));
	fread(parser->buffer, 1, parser->buffer_length, fp);

	fclose(fp);

	// set csv properties
	parser->parser_pos = 0;
	parser->columns = csv_parser_calculate_number_of_columns(parser);
	parser->lines = csv_parser_calculate_number_of_lines(parser->buffer, parser->buffer_length);
}

void csv_parser_free(csv_parser *parser) {
	free(parser->buffer);
}
