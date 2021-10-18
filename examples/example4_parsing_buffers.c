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
int main()
{
    char *buffer;
    CSV_PARSER csv;
    FILE *fp=NULL;
    size_t f_size;
    csv_parser_init(&csv,NULL);
    fp=fopen("samples/MOCK_DATA3.csv","rb");
    fseek(fp, 0L, SEEK_END);
	 f_size = ftell(fp);
	fseek(fp, 0L, SEEK_SET);
	buffer=csv_parser_malloc((f_size+1)*sizeof(*buffer),NULL);
    fread(buffer,f_size,1,fp);
    csv_parser_load_buffer(&csv,buffer,f_size);
    display_csv(csv);
    printf("%s",csv.error.reason);
    csv_parser_free(buffer,NULL);
    csv_parser_release(&csv);
    return 0;
}