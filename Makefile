CC = gcc
CFLAGS = -Wall -Wextra -g
SRC = src/
INCLUDE = include/
BIN = bin/

.PHONY: transfer
transfer: $(BIN)part1

$(BIN)part1: | $(BIN)
	$(CC) $(CFLAGS) -I$(INCLUDE) -o $@ $(SRC)main.c $(SRC)part1.c

$(BIN):
	mkdir -p $(BIN)

clean:
	rm -rf $(BIN)



