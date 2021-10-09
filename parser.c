#include "parser.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char *file_buf = NULL;
int field_offset_array[500]; // stores references to starting poistions of all the records/tokens within the file buffer
                             // maybe calculate this beforehand, or reallocate later, size should be equal to number of records

const char* gerror_message(int error_code){
    switch(error_code){
        case -1:
            return "Function call before parsing csv";
        case -2:
            return "Function call before reading file";
        case -3:
            return "Function parameter is invalid";
        default:
            return "";
    }
}

struct csv_properties {
    int col_count;
    int line_count;
    int total_records;
}csv_props;

int get_column_index_from_header_name(const char *header_name){
    if (file_buf == NULL){
        fprintf(stderr, "%s : Function call before parsing the csv\n", __PROCEDURE__);
        return -1;
    }
    int column = -3;
    for (int i = 0; i < csv_props.col_count; i++){
        char *header_in_file = file_buf + field_offset_array[i];
        if(!strcmp(header_in_file, header_name)){
            column = i;
            break;
        }
    }
    return column;
}

// PRINT FUNCTIONS

int print_record(int row, int column){
    if (file_buf == NULL){
        fprintf(stderr, "%s : Function call before parsing the csv\n", __PROCEDURE__);
        return -1;
    }
    int offset = row * csv_props.col_count + column;
    printf("%s\n", file_buf + field_offset_array[offset]);
    return 0;
}

int print_row(int row){
    if (file_buf == NULL){
        fprintf(stderr, "%s : Function call before parsing the csv\n", __PROCEDURE__);
        return -1;
    }
    if (row < 0 || row > csv_props.line_count){
        fprintf(stderr, "%s : Invalid row number\n", __PROCEDURE__);
        return -3;
    }
    int row_offset = row * csv_props.col_count;
    for (int i = 0; i < csv_props.col_count; i++){
        printf("%20s\n", file_buf + field_offset_array[row_offset + i]);
    }
    return 0;
}

int print_column(int column){
    if (file_buf == NULL){
        fprintf(stderr, "%s : Function call before parsing the csv\n", __PROCEDURE__);
        return -1;
    }
    if (column < 0 || column > csv_props.col_count){
        fprintf(stderr, "%s : Invalid column number\n", __PROCEDURE__);
        return -3;
    }
    for (int i = 0; i < csv_props.line_count; i++){
        printf("%20s\n", file_buf + field_offset_array[i * csv_props.col_count + column]);
    }
    return 0;
}

int print_csv(){
    if (file_buf == NULL){
        fprintf(stderr, "%s : Function call before parsing the csv\n", __PROCEDURE__);
        return -1;
    }
    for (int i = 0, col_itr = 1; i < csv_props.total_records; i++, col_itr++){
        if (col_itr == csv_props.col_count){
            printf("\n");
            col_itr = 0;
        }
        printf("%20s\t", file_buf + field_offset_array[i]);
    }
    return 0;
}

static int get_file_length(FILE *fp){
   fseek(fp, 0L, SEEK_END);
   int f_size = ftell(fp);
   fseek(fp, 0L, SEEK_SET);
   return f_size;
}

static int get_number_of_columns(){
    if (file_buf == NULL){
        fprintf(stderr, "%s : Function call before reading into file buffer\n", __PROCEDURE__);
        return -2;
    }
    int count = 1; // maybe use 0 for default value?
    int i = 0;
    while(file_buf[i] != '\n'){
        switch (file_buf[i]) {
            case '"': // field inside double quotes
                while (file_buf[++i] != '"');
                break;
            case '\'': // field inside single quotes
                while (file_buf[++i] != '\'');
                break;
            case ',':
                count++;
                break;
            default:
                break;
        }
        i++;
    }
    return count;
}

void parse_file(const char *path){
    FILE* fp = fopen(path, "rb");

    int file_len = get_file_length(fp);
    file_buf = malloc((file_len + 1) * sizeof *file_buf);
    fread(file_buf, 1, file_len, fp);

    fclose(fp);

    csv_props.col_count = get_number_of_columns();
    csv_props.line_count = 1;
    field_offset_array[0] = 0; // for first entry, offset is 0
    int field_offset_value = 1;

	/*
      source = "hello,world,my,name,is";
      modified_buffer = "hello\0world\0my\0name\0is";
	*/
    for (int i = 0; i < file_len; ++i){
        switch(file_buf[i]){

            case '"':
                while (file_buf[++i] != '"');
                break;

            case '\'':
                while (file_buf[++i] != '\'');
                break;

            // replace newlines and commas with \0 characters so tokens can be read at once with string operations
            case '\n':
                file_buf[i] = '\0';
                field_offset_array[field_offset_value++] = i+1;
                csv_props.line_count++;
                break;

            // TODO: implement for \r\n, test on windows
            case '\r':
                if (file_buf[i+1] == '\n') {
                    i++;
                    file_buf[i] = '\0';
                    field_offset_array[field_offset_value++] = i;
                    csv_props.line_count++;
                }
                break;

            case ',':
                file_buf[i] = '\0';
                field_offset_array[field_offset_value++] = i+1;
                break;

            default:
                break;
        }
    }
    csv_props.total_records = field_offset_value;
}

void free_csv_resources(){
    free(file_buf);
}
