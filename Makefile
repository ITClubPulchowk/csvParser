CC=gcc
OUTPUT_DIR=bin
OUTPUT=output
SRC_FILES=main.c parser.c
DEBUG=debug

$(OUTPUT_DIR)/$(OUTPUT): main.c parser.c
	@ $(CC) $^ -o $@

$(OUTPUT_DIR)/$(DEBUG): main.c parser.c
	@ $(CC) -g $^ -o $@

.PHONY: run clean debug
run: $(OUTPUT_DIR)/$(OUTPUT)
	@ $(OUTPUT_DIR)/$(OUTPUT)

debug: $(OUTPUT_DIR)/$(DEBUG)
	@ gdb --tui --quiet $<

clean:
	@ rm $(OUTPUT_DIR)/$(OUTPUT) $(OUTPUT_DIR)/$(DEBUG)
