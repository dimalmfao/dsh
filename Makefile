CC = gcc
CFLAGS = -Wall -Wextra -O2

SRC = main.c
OBJ = $(SRC:.c=.o)
EXEC = dsh

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(EXEC)

.PHONY: all clean
