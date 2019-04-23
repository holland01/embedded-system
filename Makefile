CC := gcc-arm
LD := ld-arm

FLAGS := -g -I. -O0
TARGET := hello

SRC=.
OBJ=.

SOURCES := $(wildcard $(SRC)/*.c)
OBJECTS := $(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SOURCES))

$(TARGET): $(OBJECTS)
	$(LD) -T lpc1114.ld -o $@ $^

$(OBJ)/%.o: $(SRC)/%.c
	$(CC) $(FLAGS) -c $< -o $@

