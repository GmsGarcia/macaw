output/editor: main.c | output
	$(CC) main.c -o output/editor -Wall -Wextra -pedantic -std=c17 -lncurses
