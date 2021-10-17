#define CSV_PARSER_IMPLEMENTATION
#include "../csv_parser.h"
#define CSV_DESERIALIZER_IMPLEMENTATION
#include "../csv_deserializer.h"

void get_row_in_array(CSV_PARSER csv,int row,char **row_array)
{
    int len=0,letter_count=0;
    for (int i = 0; i < row*csv.columns; i++)
    {
        csv_parser_next(&csv,&len);
        letter_count+=len;
    }
    for (int i = 0; i < csv.columns; i++)
    {
        *row_array[i]=csv_prser_next(&csv,&len);
    }
}
void get_col_in_array(CSV_PARSER csv, int col, char **col_array)
{
    int len = 0;
    for (int j = 0; j < csv.lines; j++)
    {
        for (int i = 0; i < col; i++)
        csv_parser_next(&csv, &len);

        col_array[j] = csv_parser_next(&csv, &len);
        for (int i = col+1; i < csv.columns; i++)
            csv_parser_next(&csv, &len);
    }
}
int main()
{
    CSV_PARSER csv;
    char **boolean_data;
    CSV_PARSER_Bool *col_data;
    csv_parser_init(&csv, NULL);
    csv_parser_load(&csv, "samples/MOCK_DATA0.csv");
    boolean_data=malloc(sizeof(char ** ) * csv.lines);
    for (int i = 0; i < csv.lines; i++)
    {
        boolean_data[i]=malloc(sizeof(char)*20);
    }
    col_data=malloc(sizeof(CSV_PARSER_Bool)*csv.lines);
    get_col_in_array(csv, 2, boolean_data);
    for (int i = 1; i < csv.lines; i++)
    {
        csv_deserialize_boolean(NULL, boolean_data[i], strlen(boolean_data[i]), &col_data[i]);
        col_data[i] = !col_data[i];
        printf("%s:%d\n", boolean_data[i],col_data[i]);
    }
    free(col_data);
    return 0;
}