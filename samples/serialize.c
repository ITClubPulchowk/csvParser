#define CSV_PARSER_IMPLEMENTATION
#include "../csv_parser.h"
#define CSV_SERIALIZER_IMPLEMENTATION
#include "../csv_serializer.h"


struct info
{
  char* name;
  bool compilation;
  bool execution;
  bool output;
  bool correctness;
  double size;
  double runtime;
  double memory;
};
// Example 
void print_info(struct info* str)
{
  fprintf(stdout,
	  "Name        -> %-20s\n"
	  "Compilation -> %d.\n"
	  "Execution   -> %d.\n"
	  "Output      -> %d.\n"
	  "Correctness -> %d.\n"
	  "Size        -> %g.\n"
	  "Runtime     -> %g.\n"
	  "Memory      -> %g.\n",
	  str->name,str->compilation,str->execution,str->output,str->correctness,
	  str->size,str->runtime,str->memory);
}

#define stoffset(x) offsetof(struct info, x)

int main()
{
  CSV_SERIALIZE_DESC desc;
  desc.length = 8;

  // Allocate memory 
  desc.serializer = malloc(sizeof(*desc.serializer) * desc.length);
  desc.offset = malloc(sizeof(*desc.offset) * desc.length);

  // Fill in the descriptions manually
  serializer funcs[] = {
    csv_serialize_cstr,
    csv_serialize_boolean,
    csv_serialize_boolean,
    csv_serialize_boolean,
    csv_serialize_boolean,
    csv_serialize_real,
    csv_serialize_real,
    csv_serialize_real
  };
 
  memcpy(desc.serializer,funcs,sizeof(funcs));

  
  int offsets[] = {
    stoffset(name),
    stoffset(compilation),
    stoffset(execution),
    stoffset(output),
    stoffset(correctness),
    stoffset(size),
    stoffset(runtime),
    stoffset(memory)
  };
  memcpy(desc.offset,offsets,sizeof(offsets));

  csv_parser parser;
  csv_parser_init(&parser, NULL);
  if (csv_parser_load(&parser, "./MOCK_DATA.csv")) {

    int number_of_lines_to_parse = (int)parser.lines;
    if (number_of_lines_to_parse > 100)
      number_of_lines_to_parse = 5;

    for (int col = 0; col < parser.columns; ++col) {
      size_t length = 0;
      char *value = csv_parser_next(&parser, &length);
            printf("%-12s ", value);
    }
    printf("\n");
		
    struct info record_array[10];
    int count = csv_serialize(NULL,record_array,&desc,&parser,sizeof(*record_array),5);

    for (int i = 0; i < count; ++i)
    {
      print_info(record_array+i);
      printf("\n");
    }

    csv_parser_release(&parser);
  }

  
  return 0;
};
