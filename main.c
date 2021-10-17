#define CSV_PARSER_IMPLEMENTATION
#include "csv_parser.h"
#define CSV_DESERIALIZER_IMPLEMENTATION
#include "csv_deserializer.h"
//
// Custom struct
//

typedef struct dob
{
	int year, month, day;
} dob;
typedef struct time
{
	int hour, minutes, seconds;
} time;

//
// Custom parser
//

CSV_PARSER_Bool dob_parser(char *value, size_t len, dob *out)
{
	int n = sscanf(value, "%d/%d/%d", &out->month, &out->day, &out->year);
	return n == 3;
}

CSV_PARSER_Bool time_parser(char *value, size_t len, time *out)
{
	int num = sscanf(value, "%d:%d:%d", &out->hour, &out->minutes, &out->seconds);
	if (num == 2)
	{
		num = sscanf(value, "%d:%d", &out->hour, &out->minutes);
		return num == 2;
	}
	else if (num == 1)
	{
		num = sscanf(value, "%d", &out->hour);
		return num == 1;
	}
	else
		return num == 3;
}

int main(int argc, char *argv[])
{
	CSV_PARSER csv;
	csv_parser_load(&csv, "samples\\guild-communicators.csv");
	// print_row(csv, 4);
	print_csv(csv);
	return 0;
}
