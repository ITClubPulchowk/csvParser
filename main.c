#include "parser.h"
#include <stdio.h>

int main(int argc, char *argv[]) {
	csv_parser parser;
	csv_parser_load(&parser, "./samples/guild-communicators.csv");

	for (int row = 0; row < parser.lines; ++row) {
		for (int col = 0; col < parser.columns; ++col) {
			char *value = csv_parser_next(&parser);
			printf("%s ", value);
		}
		printf("\n");
	}

	csv_parser_free(&parser);
	return 0;
}
