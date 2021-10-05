#include "parser.h"
#include <stdio.h>

int main(int argc, char *argv[]) {
	parse_file("./samples/popular-text-channels.csv");

	int column_index = get_column_index_from_header_name("channel_id");
	print_column(column_index);

	//print_column(1); // print specific column
	//print_row(2); // print specific row
	//print_csv(); // print the entire csv
	//print_record(8, 3); // print a specific record/token

	free_csv_resources();
	return 0;
}
