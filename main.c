#define CSV_PARSER_IMPLEMENTATION
#include "csv_parser.h"
#define CSV_DESERIALIZER_IMPLEMENTATION
#include "csv_deserializer.h"
#define newline printf("\n")
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
void print_row(CSV_PARSER csv, int row);
void print_col(CSV_PARSER csv, int col);
void print_csv(CSV_PARSER csv);
void print_data(int row, int col);
int main(int argc, char *argv[])
{
	CSV_PARSER csv;
	csv_parser_load(&csv, "samples\\guild-communicators.csv");
	// print_row(csv, 4);
	print_csv(csv);
	return 0;
}
void print_row(CSV_PARSER csv, int row)
{
	int i = 0, j = 0, k = 0;
	char str[100];
	while (1)
	{
		if (csv.buffer[i] == '\0')
			j++;
		if (j == csv.columns * row)
		{
			break;
		}
		i++;
	}
	for (j = 0; j <= csv.columns; i++)
	{
		str[k] = csv.buffer[i];
		if (csv.buffer[i] == '\0')
		{
			j++;
			str[k] = '\n';
		}
		k++;
	}
	str[k] = '\0';
	printf("%s", str);
}
void print_col(CSV_PARSER csv, int col)
{
	int j = 0;
	for (int i = 0; i < csv.buffer_length; i++)
	{
		if (csv.buffer[i] == '\0')
		{
			j++;
		}
		if (j == col)
		{
			printf("%c", csv.buffer[i]);
		}
		if (j == csv.columns)
			j = 0;
	}
}
void print_csv(CSV_PARSER csv)
{	
	
}