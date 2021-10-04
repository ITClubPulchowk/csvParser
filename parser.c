#include "parser.h"

#define print_newline printf("\n") // can be removed later
#define NUMBER_OF_RECORDS 100 // temporary test, later calculate by counting commas and newlines

char *file_buf = NULL;
int field_offset_array[100]; // maybe calculate this beforehand, or reallocate later

struct csv_properties {
    int col_count;
    int line_count;
    int total_records;
}csv_props;

int get_column_index_from_header_name(const char *header_name){
    if (file_buf == NULL){
        fprintf(stderr, "Please parse the csv before accessing the elements!");
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
        fprintf(stderr, "Header with the name %s does not exist!\n", header_name);
        exit(1);
    }
    return column;
}

void print_record(int row, int column){
    if (file_buf == NULL){
        fprintf(stderr, "Please parse the csv before accessing the elements!");
        exit(1);
    }
    int offset = row * csv_props.col_count + column;
    printf("%s\n", file_buf + field_offset_array[offset]);
}

void print_row(int row){
    if (file_buf == NULL){
        fprintf(stderr, "Please parse the csv before accessing the elements!");
        exit(1);
    }
    int row_offset = row * csv_props.col_count;
    for (int i = 0; i < csv_props.col_count; i++){
        printf("%s", file_buf + field_offset_array[row_offset + i]);
        print_newline;
    }
}

void print_column(int column){
    if (file_buf == NULL){
        fprintf(stderr, "Please parse the csv before accessing the elements!");
        exit(1);
    }
    for (int i = 0; i < csv_props.line_count; i++){
        printf("%s", file_buf + field_offset_array[i * csv_props.col_count + column]);
        print_newline;
    }
}

void print_csv(){
    for (int i = 0; i < csv_props.total_records; i++){
        printf("%d : %s", i+1, file_buf + field_offset_array[i]);
        print_newline;
    }
}

static int get_file_length(FILE *fp){
   fseek(fp, 0L, SEEK_END);
   int f_size = ftell(fp);
   fseek(fp, 0L, SEEK_SET);
   return f_size;
}

// TODO: blank lines?
static int get_number_of_columns(FILE *fp){
    int count = 1; // maybe use 0 for default value?
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
                count++;
                break;
            default:
                break;
        }
    }
    fseek(fp, 0L, SEEK_SET);
    return count;
}

void parse_file(const char *path){
    FILE* fp = fopen(path, "r");
    csv_props.col_count = get_number_of_columns(fp);

    int file_len = get_file_length(fp);
    file_buf = malloc((file_len + 1 ) * sizeof *file_buf);

    char ch;
    int itr = 0, field_offset_value = 1;
    field_offset_array[0] = 0;

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
                csv_props.line_count++;
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

    csv_props.total_records = field_offset_value;
    fclose(fp);
}

void free_csv_resources(){
    free(file_buf);
}
