#define CSV_PARSER_IMPLEMENTATION
#include "../csv_parser.h"
#define CSV_DESERIALIZER_IMPLEMENTATION
#include "../csv_deserializer.h"

void display_csv(CSV_PARSER csv)
{
    size_t len=0;
    for (int i = 0; i < csv.lines*csv.columns; i++)
    {
        if(i%csv.columns==0)
            printf("\n");
        printf("%20s",csv_parser_next(&csv,&len));
    }
    printf("\n");
}

int main(int argc, char const *argv[])
{
    const char *filename = NULL;
    if(argc<2)
    {
        filename = "samples/MOCK_DATA1.csv";
    }
    else
    {
        filename = argv[1];
    }

    CSV_PARSER csv;
    csv_parser_init(&csv,NULL);
    csv_parser_load(&csv,filename);
    display_csv(csv);
    csv_parser_release(&csv);
    return 0;
}

