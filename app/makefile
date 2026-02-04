CC = gcc
FLAGS = -W -Wall -pedantic
DRIVER = ../driver/drv_api.a

GEN_DIR = ../lib
GEN_LIB = $(GEN_DIR)/lib.a
GEN_INC = -I$(GEN_DIR)

SRC = app.c
HEADER = app.h

all: app

app: $(SRC) $(HEADER) $(GEN_LIB)
	$(CC) $(FLAGS) $(SRC) $(GEN_INC) $(DRIVER) $(GEN_LIB) -o app

clean:
	rm -f *.o app

.PHONY: all clean