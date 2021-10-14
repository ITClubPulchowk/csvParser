#define CSV_PARSER_IMPLEMENTATION
#include "csv_parser.h"

#include <stdio.h>

int main(int argc, char *argv[]) {
	csv_parser parser;
	csv_parser_init(&parser, NULL);
	if (csv_parser_load(&parser, "./samples/MOCK_DATA.csv")) {
		for (int row = 0; row < parser.lines; ++row) {
			for (int col = 0; col < parser.columns; ++col) {
				size_t length = 0;
				char *value = csv_parser_next(&parser, &length);
				printf("%s ", value);
			}
			printf("\n");
		}

		csv_parser_release(&parser);
	}
	return 0;
}
