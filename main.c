#include "parser.h"
#include <stdio.h>

int main(int argc, char *argv[]) {
	//char str[] = "hello\0world\0my\0name\0is\0pranjal";
	//printf("%s", str+6);
	//parse_file("./samples/popular-text-channels.csv");
	parse_file("./samples/participators-by-guild-tenure.csv");
	//printf("Nothing interesting here right now...\n");
	return 0;
}
