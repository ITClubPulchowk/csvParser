#define CSV_PARSER_IMPLEMENTATION
#include "../csv_parser.h"
#define CSV_DESERIALIZER_IMPLEMENTATION
#include "../csv_deserializer.h"
typedef struct row_data{
    uint8_t *name;
    CSV_PARSER_Bool compilation;
    CSV_PARSER_Bool execution;
    CSV_PARSER_Bool output;
    CSV_PARSER_Bool correctness;
    double size;
    double runtime;
    double memory;
}row_data;
int main()
{
    CSV_DESERIALIZE_DESC desc;
    CSV_Deserializer funcs[] = {
        csv_deserialize_stringdup,
        csv_deserialize_boolean,
        csv_deserialize_boolean,
        csv_deserialize_boolean,
        csv_deserialize_boolean,
        csv_deserialize_real,
        csv_deserialize_real,
        csv_deserialize_real
    };
    size_t offsets[] = {
        offsetof(row_data,name),
        offsetof(row_data,execution),
        offsetof(row_data,compilation),
        offsetof(row_data,output),
        offsetof(row_data,correctness),
        offsetof(row_data,size),
        offsetof(row_data,runtime),
        offsetof(row_data,memory)
    };
    CSV_PARSER csv;
    row_data *data1;
    csv_parser_init(&csv,NULL);
    csv_parser_load(&csv,"samples/MOCK_DATA0.csv");
    desc.deserializer=funcs;
    desc.offset=offsets;
    desc.length=csv.columns;
    data1=malloc(sizeof(row_data)*csv.lines);
    csv_parser_skip_row(&csv);
    csv_deserialize(NULL,data1,&desc,sizeof(row_data),&csv,csv.lines);
    for (int i = 0; i < csv.lines; i++)
    {
        printf("%s\n",data1[i].name);
    }
    free(data1);
    return 0;
}