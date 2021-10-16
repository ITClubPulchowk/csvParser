# csvParser

## csv_parser.h
csvParser is header only library, to use this, just include `csv_parser.h`. Example code is as follows:
```c
#define CSV_PARSER_IMPLEMENTATION // This must only be one in one C/C++ file, forces to include implementation
#include "csv_parser.h" // Include declarations only if CSV_PARSER_IMPLEMENTATION is not defined

#include <stdio.h>

int main(int argc, char *argv[]) {
	if (argc != 2) {
		fprintf(stderr, "Invalid. USAGE: %s <csv_file>\n", argv[0]);
		return 1;
	}

	csv_parser parser;
	csv_parser_init(&parser, NULL);
	if (csv_parser_load(&parser, argv[1])) {
		for (int row = 0; row < parser.lines; ++row) {
			for (int col = 0; col < parser.columns; ++col) {
				char *value = csv_parser_next(&parser);
				printf("%s ", value);
			}
			printf("\n");
		}

		csv_parser_release(&parser);
	}
	return 0;
}
```

## csv_deserializer.h
This is header only library, to use this, just include this file.
This header is dependent to csv_parser.h and so csv_parser.h must be in the Includes directory for this library to work.
This header must be included after csv_parser.h is included.
```c
#define CSV_DESERIALIZER_IMPLEMENTATION // This must only be one in one C/C++ file, forces to include implementation.
#include "csv_deserializer.h" // Include declarations only if CSV_DESERIALIZER_IMPLEMENTATION is not defined

struct info
{
	char *name;
	CSV_PARSER_Bool compilation;
	CSV_PARSER_Bool execution;
	CSV_PARSER_Bool output;
	CSV_PARSER_Bool correctness;
	double size;
	double runtime;
	double memory;
};

#define stoffset(x) offsetof(struct info,x)

void print_info(struct info *str)
{
	fprintf(stdout,
		"Name        -> %-20s\n"
		"compilation -> %d.\n"
		"execution   -> %d.\n"
		"output      -> %d.\n"
		"correctness -> %d.\n"
		"size        -> %g.\n"
		"runtime     -> %g.\n"
		"memory      -> %g.\n",
		str->name, str->compilation, str->execution, str->output, str->correctness,
		str->size, str->runtime, str->memory);
}

struct info test[100];

int main()
{
	CSV_DESERIALIZE_DESC desc;

	CSV_Deserializer funcs[] = {
		csv_deserialize_stringdup,
		csv_deserialize_boolean,
		csv_deserialize_boolean,
		csv_deserialize_boolean,
		csv_deserialize_boolean,
		csv_deserialize_real,
		csv_deserialize_real,
		csv_deserialize_real
	};

	size_t offsets[] = {
		stoffset(name),
		stoffset(compilation),
		stoffset(execution),
		stoffset(output),
		stoffset(correctness),
		stoffset(size),
		stoffset(runtime),
		stoffset(memory)
	};

	desc.deserializer = funcs;
	desc.offset = offsets;
	desc.length = 8;

	CSV_PARSER parser;
	csv_parser_init(&parser, NULL);
	if (csv_parser_load(&parser, "./samples/MOCK_DATA.csv")) {
		size_t number_of_lines_to_parse = parser.lines;
		if (number_of_lines_to_parse > 100)
			number_of_lines_to_parse = 100;

		for (int col = 0; col < parser.columns; ++col) {
			size_t length = 0;
			char *value = csv_parser_next(&parser, &length);
			// printf("%-12s ", value);
		}
		printf("\n");

		csv_deserialize(NULL, (void *)&test, &desc, sizeof(struct info), &parser, number_of_lines_to_parse);
		for (int row = 0; row < number_of_lines_to_parse; ++row) {
			print_info(&test[row]); // --> :: Destroy char* of struct obtained from this function
			fprintf(stdout, "\n");
		}

		csv_parser_release(&parser);
	}

	return 0;
}
```


# Examples to code
1. Using serial parsing by printing in table
2. Using serializer to parse structs with not complex types
3. Using custom parser functions to parse complex types such as array / structs
4. Examples of replacing std library function, only allocation functions
5. Examples by loading file and reading by ourself instead of using the function that automatically loads the file and parses it
