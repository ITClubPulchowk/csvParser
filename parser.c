#include "parser.h"

char *file_buf = NULL;
int field_offset_array[500]; // stores references to starting poistions of all the records/tokens within the file buffer
                             // maybe calculate this beforehand, or reallocate later, size should be equal to number of records

struct csv_properties {
    int col_count;
    int line_count;
    int total_records;
}csv_props;

int get_column_index_from_header_name(const char *header_name){
    if (file_buf == NULL){
        fprintf(stderr, "%s : Function call before parsing the csv\n", __PROCEDURE__);
        exit(1);
    }
    int column = -1;
    for (int i = 0; i < csv_props.col_count; i++){
        char *header_in_file = file_buf + field_offset_array[i];
        if(!strcmp(header_in_file, header_name)){
            column = i;
            break;
        }
    }
    if (column == -1){
        fprintf(stderr, "%s : Header with name %s does not exist!\n", __PROCEDURE__, header_name);
        exit(1);
    }
    return column;
}

// PRINT FUNCTIONS

void print_record(int row, int column){
    if (file_buf == NULL){
        fprintf(stderr, "%s : Function call before parsing the csv\n", __PROCEDURE__);
        exit(1);
    }
    int offset = row * csv_props.col_count + column;
    printf("%s\n", file_buf + field_offset_array[offset]);
}

void print_row(int row){
    if (file_buf == NULL){
        fprintf(stderr, "%s : Function call before parsing the csv\n", __PROCEDURE__);
        exit(1);
    }
    int row_offset = row * csv_props.col_count;
    for (int i = 0; i < csv_props.col_count; i++){
        printf("%s\n", file_buf + field_offset_array[row_offset + i]);
    }
}

void print_column(int column){
    if (file_buf == NULL){
        fprintf(stderr, "%s : Function call before parsing the csv\n", __PROCEDURE__);
        exit(1);
    }
    for (int i = 0; i < csv_props.line_count; i++){
        printf("%s\n", file_buf + field_offset_array[i * csv_props.col_count + column]);
    }
}

void print_csv(){
    if (file_buf == NULL){
        fprintf(stderr, "%s : Function call before parsing the csv\n", __PROCEDURE__);
        exit(1);
    }
    for (int i = 0, col_itr = 1; i < csv_props.total_records; i++, col_itr++){
        if (col_itr == csv_props.col_count){
            printf("\n");
            col_itr = 0;
        }
        printf("%s\t", file_buf + field_offset_array[i]);
    }
}

static int get_file_length(FILE *fp){
   fseek(fp, 0L, SEEK_END);
   int f_size = ftell(fp);
   fseek(fp, 0L, SEEK_SET);
   return f_size;
}

// TODO: blank lines?
static int get_number_of_columns(){
    if (file_buf == NULL){
        fprintf(stderr, "%s : Function call before reading into file buffer\n", __PROCEDURE__);
        exit(1);
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
    FILE* fp = fopen(path, "r");

    int file_len = get_file_length(fp);
    file_buf = malloc((file_len + 1) * sizeof *file_buf);
    fread(file_buf, 1, file_len, fp);
    fclose(fp);

    csv_props.col_count = get_number_of_columns();
    csv_props.line_count = 1;

    char ch;
    int itr = 0, field_offset_value = 1;
    field_offset_array[0] = 0;

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
