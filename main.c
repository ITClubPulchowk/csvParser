#include "parser.h"
#include <stdio.h>

int main(int argc, char *argv[]) {
	csv_parser parser;
	load_file_buffer(&parser, "./samples/guild-communicators.csv");

	for (int row = 0; row < parser.line_count; ++row) {
		for (int col = 0; col < parser.col_count; ++col) {
			char *value = parse_next(&parser);
			printf("%s ", value);
		}
		printf("\n");
	}

	free_csv_resources(&parser);
	return 0;
}
