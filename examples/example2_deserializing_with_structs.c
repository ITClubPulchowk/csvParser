#define CSV_PARSER_IMPLEMENTATION
#include "../csv_parser.h"
#define CSV_DESERIALIZER_IMPLEMENTATION
#include "../csv_deserializer.h"
typedef struct dob
{
	int year, month, day;
} dob;
typedef struct time
{
	int hour, minutes, seconds;
} time;

typedef struct row_data{
    int id;
    uint8_t *first_name;
    dob DOB;
    time birth_time;
}row_data;
CSV_PARSER_Bool csv_deserialize_dob(void *context,uint8_t *value, size_t len, dob *out)
{
	int n = sscanf(value, "%d/%d/%d", &out->month, &out->day, &out->year);
	return n == 3;
}

CSV_PARSER_Bool csv_deserialize_time(void *context,char *value, size_t len, time *out)
{
	int num = sscanf(value, "%d:%d:%d", &out->hour, &out->minutes, &out->seconds);
	if (num == 2)
	{
		num = sscanf(value, "%d:%d", &out->hour, &out->minutes);
        out->seconds=0;
		return num == 2;
	}
	else if (num == 1)
	{
        out->minutes=0;
        out->seconds=0;
		num = sscanf(value, "%d", &out->hour);
		return num == 1;
	}
	else
		return num == 3;
}

int main()
{
    CSV_DESERIALIZE_DESC desc;
    CSV_Deserializer funcs[] = {
        csv_deserialize_sint,
        csv_deserialize_string,
        csv_deserialize_dob,
        csv_deserialize_time
    };
    size_t offsets[] = {
        offsetof(row_data,id),
        offsetof(row_data,first_name),
        offsetof(row_data,DOB),
        offsetof(row_data,birth_time),
    };
    CSV_PARSER csv;
    row_data *data1;
    csv_parser_init(&csv,NULL);
    csv_parser_load(&csv,"samples/MOCK_DATA3.csv");
    desc.deserializer=funcs;
    desc.offset=offsets;
    desc.length=csv.columns;
    data1=malloc(sizeof(row_data)*csv.lines);
    csv_parser_skip_row(&csv);
    csv_deserialize(NULL,data1,&desc,sizeof(row_data),&csv,csv.lines);
    for (int i = 0; i < csv.lines; i++)
    {
        printf("%d\t",data1[i].id);
        printf("%s\t",data1[i].first_name);
        printf("%d-%d-%d\t",data1[i].DOB.year,data1[i].DOB.month,data1[i].DOB.day);
        printf("%d:%d:%d\n",data1[i].birth_time.hour,data1[i].birth_time.minutes,data1[i].birth_time.seconds);
    }
    csv_parser_release(&csv);

    return 0;
}