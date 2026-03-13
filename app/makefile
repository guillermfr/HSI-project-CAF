CC = gcc
FLAGS = -W -Wall -pedantic
DRIVER = ../driver/drv_api.a

GEN_DIR = ../lib
GEN_LIB = $(GEN_DIR)/lib.a
GEN_INC = -I$(GEN_DIR)

FSM_DIR = ../fsm
FSM_SRC = \
	$(FSM_DIR)/fsm_feux_classiques.c \
	$(FSM_DIR)/fsm_feux_clignotants_warnings.c \
	$(FSM_DIR)/fsm_essuie-glaces_lave_glace.c

SRC = app.c $(FSM_SRC)
HEADER = app.h

all: app

app: $(SRC) $(HEADER) $(GEN_LIB)
	$(CC) $(FLAGS) $(SRC) $(GEN_INC) $(DRIVER) $(GEN_LIB) -o app

clean:
	rm -f *.o app

.PHONY: all clean