#define CSV_PARSER_IMPLEMENTATION
#include "csv_parser.h"

#include <stdio.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Invalid. USAGE: %s <csv_file>\n", argv[0]);
        return 1;
    }

    size_t len;

    CSV_PARSER parser;
    csv_parser_init(&parser, NULL);
    if (csv_parser_load(&parser, argv[1])) {
        for (int row = 0; row < parser.lines; ++row) {
            for (int col = 0; col < parser.columns; ++col) {
                char *value = csv_parser_next(&parser, &len);
                printf("%s ", value);
            }
            printf("\n");
        }

        csv_parser_release(&parser);
    }
    return 0;
}
