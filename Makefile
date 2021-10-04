CC=gcc
OUTPUT_DIR=bin
OUTPUT=output
SRC_FILES=main.c parser.c

$(OUTPUT_DIR)/$(OUTPUT): main.c parser.c
	@ $(CC) $^ -o $@

.PHONY: run
run: $(OUTPUT_DIR)/$(OUTPUT)
	@ $(OUTPUT_DIR)/$(OUTPUT)
