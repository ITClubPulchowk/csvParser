#include "parser.h"
#include <stdio.h>

int main(int argc, char *argv[]) {
	csv_parser parser = csv_defaults;
	load_file_buffer(&parser, "./samples/guild-communicators.csv");

	//int column_index = get_column_index_from_header_name(&parser, "visitors");
	//print_column(&parser, column_index);

	//print_column(&parser, 2); // print specific column
	//print_row(&parser, 2); // print specific row
	//print_csv(&parser); // print the entire csv
	//print_record(&parser, 0, 5); // print a specific record/token

	free_csv_resources(&parser);
	return 0;
}
