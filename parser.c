#include "parser.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

csv_parser csv_defaults = {.col_count = 1,
                           .line_count = 1,
                           .total_records = 0,
                           .field_offset_array = NULL,
                           .file_buf = NULL};

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

int get_column_index_from_header_name(csv_parser* parser, const char *header_name){
    if (parser->file_buf == NULL){
        fprintf(stderr, "%s : Function call before parsing the csv\n", __PROCEDURE__);
        return -1;
    }
    int column = -3;
    for (int i = 0; i < parser->col_count; i++){
        char *header_in_file = parser->file_buf + parser->field_offset_array[i];
        if(!strcmp(header_in_file, header_name)){
            column = i;
            break;
        }
    }
    return column;
}

// PRINT FUNCTIONS

int print_record(csv_parser* parser, int row, int column){
    if (parser->file_buf == NULL){
        fprintf(stderr, "%s : Function call before parsing the csv\n", __PROCEDURE__);
        return -1;
    }
    int offset = row * parser->col_count + column;
    printf("%s\n", parser->file_buf + parser->field_offset_array[offset]);
    return 0;
}

int print_row(csv_parser* parser, int row){
    if (parser->file_buf == NULL){
        fprintf(stderr, "%s : Function call before parsing the csv\n", __PROCEDURE__);
        return -1;
    }
    if (row < 0 || row > parser->line_count){
        fprintf(stderr, "%s : Invalid row number\n", __PROCEDURE__);
        return -3;
    }
    int row_offset = row * parser->col_count;
    for (int i = 0; i < parser->col_count; i++){
        printf("%20s\n", parser->file_buf + parser->field_offset_array[row_offset + i]);
    }
    return 0;
}

int print_column(csv_parser* parser, int column){
    if (parser->file_buf == NULL){
        fprintf(stderr, "%s : Function call before parsing the csv\n", __PROCEDURE__);
        return -1;
    }
    if (column < 0 || column > parser->col_count){
        fprintf(stderr, "%s : Invalid column number\n", __PROCEDURE__);
        return -3;
    }
    for (int i = 0; i < parser->line_count; i++){
        char *temp = parser->file_buf + parser->field_offset_array[i * parser->col_count + column];
        printf("%20s\n", temp);
        //printf("%d ", field_offset_array[i * csv_props.col_count + column]);
    }
    return 0;
}

int print_csv(csv_parser* parser){
    if (parser->file_buf == NULL){
        fprintf(stderr, "%s : Function call before parsing the csv\n", __PROCEDURE__);
        return -1;
    }
    for (int i = 0, col_itr = 1; i < parser->total_records; i++, col_itr++){
        if (col_itr == parser->col_count){
            printf("\n");
            col_itr = 0;
        }
        printf("%20s\t", parser->file_buf + parser->field_offset_array[i]);
    }
    return 0;
}

static int get_file_length(FILE *fp){
   fseek(fp, 0L, SEEK_END);
   int f_size = ftell(fp);
   fseek(fp, 0L, SEEK_SET);
   return f_size;
}

static int get_number_of_columns(csv_parser* parser){
    if (parser->file_buf == NULL){
        fprintf(stderr, "%s : Function call before reading into file buffer\n", __PROCEDURE__);
        return -2;
    }
    int count = 1; // maybe use 0 for default value?
    int i = 0;
    while(parser->file_buf[i] != '\n'){
        switch (parser->file_buf[i]) {
            case '"': // field inside double quotes
                while (parser->file_buf[++i] != '"');
                break;
            case '\'': // field inside single quotes
                while (parser->file_buf[++i] != '\'');
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

// TODO: get this threaded and running
static int get_number_of_lines(char *buffer, int len){
    int line_count = 1;
    for (int i = 0; i < len; i++) {
        switch(buffer[i]){
            case '\n':
                line_count++;
                break;

            case '\r':
                // check for possible overflow?
                if (i+1 < len && buffer[i+1] == '\n'){
                   line_count++;
                   i++;
                }
                break;

            default:
                break;
        }
    }
    return line_count;
}

void load_file_buffer(csv_parser* parser, const char *file_path){
    FILE* fp = fopen(file_path, "rb");

    int file_len = get_file_length(fp);
    parser->file_buf = malloc((file_len + 1) * sizeof *parser->file_buf);
    fread(parser->file_buf, 1, file_len, fp);

    fclose(fp);

    // set csv properties
    parser->col_count = get_number_of_columns(parser);
    parser->line_count = get_number_of_lines(parser->file_buf, file_len);
    parser->total_records = parser->col_count * parser->line_count;
    parser->field_offset_array = malloc(parser->total_records * sizeof *parser->field_offset_array);

    parser->field_offset_array[0] = 0; // for first entry, offset is 0
    int field_offset_value = 1;

	/*
      source = "hello,world,my,name,is";
      modified_buffer = "hello\0world\0my\0name\0is";
	*/
    for (int i = 0; i < file_len; ++i){
        switch(parser->file_buf[i]){

            case '"':
                while (parser->file_buf[++i] != '"');
                break;

            case '\'':
                while (parser->file_buf[++i] != '\'');
                break;

            // replace newlines and commas with \0 characters so tokens can be read at once with string operations
            case '\n':
                parser->file_buf[i] = '\0';
                parser->field_offset_array[field_offset_value++] = ++i;
                break;

            // TODO: implement for \r\n, test on windows
            case '\r':
                if (i+1 < file_len && parser->file_buf[i+1] == '\n') {
                    i++;
                    parser->file_buf[i] = '\0';
                    parser->field_offset_array[field_offset_value++] = ++i;
                }
                break;

            case ',':
                parser->file_buf[i] = '\0';
                parser->field_offset_array[field_offset_value++] = ++i;
                break;

            default:
                break;
        }
    }
}

void free_csv_resources(csv_parser* parser){
    free(parser->file_buf);
    free(parser->field_offset_array);
}
