#include "parser.h"
#include <stdio.h>

int main(int argc, char *argv[]) {
	csv_parser parser;
	load_file_buffer(&parser, "./samples/guild-communicators.csv");

	//for (int i = 0; i < 8; i++){
	//	printf("%s\n", parse_next(&parser));
	//}

	free_csv_resources(&parser);
	return 0;
}
