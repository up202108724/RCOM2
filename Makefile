CC = gcc
CFLAGS = -Wall -Wextra -g
SRC = src/
INCLUDE = include/
BIN = bin/

.PHONY: transfer
transfer: $(BIN)part1

$(BIN)part1: $(SRC)main.c $(SRC)part1.c
	$(CC) $(CFLAGS) -I$(INCLUDE) -o $@ $^

