#define CSV_PARSER_IMPLEMENTATION
#include "csv_parser.h"

#include <stdio.h>

typedef CSV_PARSER_Bool(*csv_parse_proc)(char *value, size_t len, void *out);

typedef struct CSV_PARSER_STRING {
	uint8_t *data;
	size_t len;
} CSV_PARSER_STRING;

CSV_PARSER_Bool csv_deserialize_length_string(char *value, size_t len, CSV_PARSER_STRING *out) {
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

CSV_PARSER_Bool csv_deserialize_string(char *value, size_t len, char **out) {
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

CSV_PARSER_Bool csv_deserialize_sint(char *value, size_t len, int radix, int64_t *out) {
	char *end = NULL;
	long long num = strtoll(value, &end, radix);
	if (value + len == end) {
		*out = (int64_t)num;
		return 1;
	}
	return 0;
}

CSV_PARSER_Bool csv_deserialize_uint(char *value, size_t len, int radix, uint64_t *out) {
	char *end = NULL;
	long long num = strtoull(value, &end, radix);
	if (value + len == end) {
		*out = (uint64_t)num;
		return 1;
	}
	return 0;
}

CSV_PARSER_Bool csv_parse_real(char *value, size_t len, double *out) {
	char *end = NULL;
	double num = strtod(value, &end);
	if (value + len == end) {
		*out = num;
		return 1;
	}
	return 0;
}

CSV_PARSER_Bool csv_parse_boolean(char *value, size_t len, CSV_PARSER_Bool *out) {
	switch (len) {
	case 1:
		*out = (value[0] == '1');
		return value[0] == '0' || value[0] == '1';

	case 4:
		if ((value[0] == 't' || value[0] == 'T') &&
			(value[1] == 'r' || value[1] == 'R') &&
			(value[2] == 'u' || value[2] == 'U') &&
			(value[3] == 'e' || value[3] == 'E')) {
			*out = 1;
			return 1;
		}

	case 5:
		if ((value[0] == 'f' || value[0] == 'F') &&
			(value[1] == 'a' || value[1] == 'A') &&
			(value[2] == 'l' || value[2] == 'L') &&
			(value[3] == 's' || value[3] == 'S') &&
			(value[4] == 'e' || value[4] == 'E')) {
			*out = 0;
			return 1;
		}
	}

	return 0;
}

//
// Custom struct
//

typedef struct dob {
	int year, month, day;
} dob;
typedef struct time{
	int hour,minutes,seconds;
}time;
//
// Custom parser
//

CSV_PARSER_Bool my_dob_parser(char *value, size_t len, dob *out) {
	int n = sscanf(value, "%d/%d/%d", &out->month, &out->day, &out->year);
	return n == 3;
}
CSV_PARSER_Bool time_parser(char *value,size_t len,time *out)
{
	int num=sscanf(value,"%d:%d:%d",&out->hour,&out->minutes,&out->seconds);
	if(num==2)
	{
		num=sscanf(value,"%d:%d",&out->hour,&out->minutes);
		return num==2;
	}
	else if(num==1)
	{
		num=sscanf(value,"%d",&out->hour);
		return num==1;
	}
	else
	return num==3;
}

int main(int argc, char *argv[]) {
	// test
	csv_parse_proc user_parse_proc = (csv_parse_proc)my_dob_parser;

	CSV_PARSER parser;
	csv_parser_init(&parser, NULL);
	if (csv_parser_load(&parser, "./samples/MOCK_DATA.csv")) {
		for (int col = 0; col < parser.columns; ++col) {
			size_t length = 0;
			char *value = csv_parser_next(&parser, &length);
			printf("%-12s ", value);
		}
		printf("\n");
		
		for (int row = 1; row < parser.lines; ++row) {
			for (int col = 0; col < parser.columns; ++col) {
				size_t length = 0;
				char *value = csv_parser_next(&parser, &length);

				// hard coding this for now
				switch (col) {
					case 0: {
						char *out;
						assert(csv_deserialize_string(value, length, &out));
						printf("%-12s ", out);
					} break;

#if 0
					case 1: {
						dob d;
						assert(user_parse_proc(value, length, &d));
						printf("%d-%d-%d", d.day, d.month, d.year);
					} break;

#else
					case 1:
					case 2:
					case 3:
					case 4: {
						CSV_PARSER_Bool out;
						assert(csv_parse_boolean(value, length, &out));
						printf("%-12s ", out ? "true" : "false");
					} break;

					default: {
						double out;
						assert(csv_parse_real(value, length, &out));
						printf("%-12.3f ", out);
					} break;
#endif
				}
			}
			printf("\n");
		}

		csv_parser_release(&parser);
	}
	else {
		if (parser.error.line && parser.error.column) {
			printf("Line: %zu, Column: %zu :: ", parser.error.line, parser.error.column);
		}
		printf("Error: %s\n", parser.error.reason);
	}

	return 0;
}
