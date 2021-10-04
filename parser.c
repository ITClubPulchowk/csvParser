#include "parser.h"
#include <stdlib.h>
#include <stdbool.h>

#define print_newline printf("\n") // can be removed later
#define NUMBER_OF_RECORDS 100 // temporary test, later calculate by counting commas and newlines

bool contains_header = true; // csv file contains header for field names
int field_offset_array[100]; // maybe calculate this beforehand

enum FieldType {
STRING,
INTEGER,
DOUBLE
};

static int get_file_length(FILE *fp){
   fseek(fp, 0L, SEEK_END);
   int f_size = ftell(fp);
   fseek(fp, 0L, SEEK_SET);

   return f_size;
}

// TODO: blank lines?
static int get_number_of_columns(FILE *fp){
    int col_count = 1; // maybe use 0 for default value?
    char ch;
    while((ch = getc(fp)) != '\n'){
        switch (ch) {
            case '"': // field inside double quotes
                while (getc(fp) != '"');
                break;
            case '\'': // field inside single quotes
                while (getc(fp) != '\'');
                break;
            case ',':
                col_count++;
                break;
            default:
                break;
        }
    }
    fseek(fp, 0L, SEEK_SET);
    return col_count;
}

void parse_file(const char *path){
    FILE* fp = fopen(path, "r");
    int file_len = get_file_length(fp);

    char *file_buf = malloc((file_len + 1 ) * sizeof *file_buf);
    int line_count = 0;
    char ch;
    int itr = 0, field_offset_value = 0;

    while ((ch = fgetc(fp)) != EOF){
        switch(ch){

            case '"':
                file_buf[itr++] = ch;
                while ((ch = getc(fp)) != '"'){
                    file_buf[itr++] = ch;
                }
                file_buf[itr++] = ch;
                break;

            case '\'':
                file_buf[itr++] = ch;
                while ((ch = getc(fp)) != '\''){
                    file_buf[itr++] = ch;
                }
                file_buf[itr++] = ch;
                break;

            case '\n':
                file_buf[itr++] = '\0';
                field_offset_array[field_offset_value++] = itr;
                line_count++;
                break;

            case ',':
                file_buf[itr++] = '\0';
                field_offset_array[field_offset_value++] = itr;
                break;

            default:
                file_buf[itr++] = ch;
                break;
        }
    }

    fclose(fp);

    printf("0 : %s\n", file_buf);
    for (int i = 0; i < field_offset_value; i++){
        printf("%d : %s", i+1, file_buf + field_offset_array[i]);
        print_newline;
    }
}
