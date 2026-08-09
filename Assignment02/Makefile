CC = gcc
CFLAGS = -Wall -Wextra -O2 -std=c11 -D_POSIX_C_SOURCE=199309L -I../utilities -Isrc -Idriver

BF_SRC = ../utilities/csr.c src/bellman_ford.c driver/bf_driver.c
BF_BIN = bellman_ford

FW_SRC = src/floyd_warshall.c driver/fw_driver.c
FW_BIN = floyd_warshall

all: $(BF_BIN) $(FW_BIN)

$(BF_BIN): $(BF_SRC)
	$(CC) $(CFLAGS) -o $(BF_BIN) $(BF_SRC)

$(FW_BIN): $(FW_SRC)
	$(CC) $(CFLAGS) -o $(FW_BIN) $(FW_SRC)

clean:
	rm -f $(BF_BIN) $(FW_BIN)

.PHONY: all clean