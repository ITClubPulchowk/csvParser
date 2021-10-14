#define CSV_PARSER_IMPLEMENTATION
#include "csv_parser.h"

#include <stdio.h>

typedef struct csv_parser_string {
	uint8_t *data;
	size_t len;
} csv_parser_string;

csv_parser_bool csv_parse_length_string(char *value, size_t len, csv_parser_string *out) {
	if (len) {
		if (value[0] != '"') {
			out->data = value;
			out->len = len;
			return 1;
		}

		value[len - 1] = 0; // removing quote at the end
		out->data = value + 1;
		out->len = len - 1;
		return 1;
	}
	return 0;
}

csv_parser_bool csv_parse_string(char *value, size_t len, char **out) {
	if (len) {
		if (value[0] != '"') {
			*out = value;
			return 1;
		}

		value[len - 1] = 0; // removing quote at the end
		*out = value + 1;
		return 1;
	}
	return 0;
}

csv_parser_bool csv_parse_sint(char *value, size_t len, int radix, int64_t *out) {
	char *end = NULL;
	long long num = strtoll(value, &end, radix);
	if (value + len == end) {
		*out = (int64_t)num;
		return 1;
	}
	return 0;
}

csv_parser_bool csv_parse_uint(char *value, size_t len, int radix, uint64_t *out) {
	char *end = NULL;
	long long num = strtoull(value, &end, radix);
	if (value + len == end) {
		*out = (uint64_t)num;
		return 1;
	}
	return 0;
}

csv_parser_bool csv_parse_real(char *value, size_t len, double *out) {
	char *end = NULL;
	double num = strtod(value, &end);
	if (value + len == end) {
		*out = num;
		return 1;
	}
	return 0;
}

csv_parser_bool csv_parse_boolean(char *value, size_t len, csv_parser_bool *out) {
	switch (len) {
	case 1:
		*out = (value[0] == '1');
		return value[0] == '0' || value[0] == '1';

	case 4:
		if ((value[0] = 't' || value[0] == 'T') &&
			(value[0] = 'r' || value[0] == 'R') &&
			(value[0] = 'u' || value[0] == 'U') &&
			(value[0] = 'e' || value[0] == 'E')) {
			*out = 1;
			return 1;
		}

	case 5:
		if ((value[0] = 'f' || value[0] == 'F') &&
			(value[0] = 'a' || value[0] == 'A') &&
			(value[0] = 'l' || value[0] == 'L') &&
			(value[0] = 's' || value[0] == 'S') &&
			(value[0] = 'e' || value[0] == 'E')) {
			*out = 0;
			return 1;
		}
	}

	return 0;
}

int main(int argc, char *argv[]) {
	csv_parser parser;
	csv_parser_init(&parser, NULL);
	if (csv_parser_load(&parser, "./samples/MOCK_DATA.csv")) {

		int number_of_lines_to_parse = (int)parser.lines;
		if (number_of_lines_to_parse > 100)
			number_of_lines_to_parse = 100;

		for (int col = 0; col < parser.columns; ++col) {
			size_t length = 0;
			char *value = csv_parser_next(&parser, &length);
			printf("%-12s ", value);
		}
		printf("\n");
		
		for (int row = 1; row < number_of_lines_to_parse; ++row) {
			for (int col = 0; col < parser.columns; ++col) {
				size_t length = 0;
				char *value = csv_parser_next(&parser, &length);

				// hard coding this for now
				switch (col) {
					case 0: {
						char *out;
						assert(csv_parse_string(value, length, &out));
						printf("%-12s ", out);
					} break;

					case 1:
					case 2:
					case 3:
					case 4: {
						csv_parser_bool out;
						assert(csv_parse_boolean(value, length, &out));
						printf("%-12s ", out ? "true" : "false");
					} break;

					default: {
						double out;
						assert(csv_parse_real(value, length, &out));
						printf("%-12.3f ", out);
					} break;
				}
			}
			printf("\n");
		}

		csv_parser_release(&parser);
	}
	return 0;
}
