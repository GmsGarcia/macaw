# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -Wextra -pedantic -std=c17 -g

# Libraries
LIBS = -lncurses

# Output binary
OUTPUT = output/editor

# Source files
SRCS = main.c src/editor.c src/buffer.c src/input.c src/render.c src/color.c

# Object files (replace .c with output/.o)
OBJS = $(SRCS:%.c=output/%.o)

# Default target
all: $(OUTPUT)

# Link the object files to create the final executable
$(OUTPUT): $(OBJS)
	$(CC) $(OBJS) -o $(OUTPUT) $(LIBS)

# Compile source files into object files in the output/ directory
output/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up build files
clean:
	rm -rf output/*.o $(OUTPUT)
