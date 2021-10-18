#define CSV_PARSER_IMPLEMENTATION
#include "../csv_parser.h"
#define CSV_DESERIALIZER_IMPLEMENTATION
#include "../csv_deserializer.h"
void display_csv(CSV_PARSER csv)
{
    int len=0;
    for (int i = 0; i < csv.lines*csv.columns; i++)
    {
        if(i%csv.columns==0)
            printf("\n");
        printf("%20s",csv_parser_next(&csv,&len));
    }
    
}
int main(int argc, char const *argv[])
{
    CSV_PARSER csv;
    const char filename[50];
    csv_parser_init(&csv,NULL);
    if(argc<2)
    {
        printf("please enter filename as argument");
    }
    else{
    csv_parser_load(&csv,argv[1]);
    display_csv(csv);}
    return 0;
}

