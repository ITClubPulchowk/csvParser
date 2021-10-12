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

static int get_file_length(FILE *fp) {
	fseek(fp, 0L, SEEK_END);
	int f_size = ftell(fp);
	fseek(fp, 0L, SEEK_SET);
	return f_size;
}

static int get_number_of_columns(csv_parser *parser) {
	if (parser->file_buf == NULL) {
		fprintf(stderr, "%s : Function call before reading into file buffer\n", __PROCEDURE__);
		return -2;
	}
	int count = 1; // maybe use 0 for default value?
	int i = 0;
	while (parser->file_buf[i] != '\n') {
		switch (parser->file_buf[i]) {
		case '"': // field inside double quotes
			while (parser->file_buf[++i] != '"');
			break;
		case '\'': // field inside single quotes
			while (parser->file_buf[++i] != '\'');
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
static int get_number_of_lines(char *buffer, int len) {
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
	char *next_token = parser->file_buf + parser->parser_pos;
	int parse_status = 1;
	for (int i = parser->parser_pos; parse_status && i < parser->file_len; i++) {
		switch (parser->file_buf[i]) {
		case '"':
			while (parser->file_buf[++i] != '"');
			break;

		case '\'':
			while (parser->file_buf[++i] != '\'');
			break;

		case '\n':
			parser->file_buf[i] = '\0';
			parser->parser_pos = ++i;
			parse_status = 0;
			break;

			// TODO: implement for \r\n, test on windows
		case '\r':
			if (i + 1 < parser->file_len && parser->file_buf[i + 1] == '\n') {
				i++;
				parser->file_buf[i] = '\0';
				parser->parser_pos = ++i;
				parse_status = 0;
			}
			break;

		case ',':
			parser->file_buf[i] = '\0';
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

	parser->file_len = get_file_length(fp);
	parser->file_buf = malloc((parser->file_len + 1) * sizeof * parser->file_buf);
	fread(parser->file_buf, 1, parser->file_len, fp);

	fclose(fp);

	// set csv properties
	parser->parser_pos = 0;
	parser->columns = get_number_of_columns(parser);
	parser->lines = get_number_of_lines(parser->file_buf, parser->file_len);
}

void csv_parser_free(csv_parser *parser) {
	free(parser->file_buf);
}
