CC := gcc-arm
LD := ld-arm

INCLUDE=./include

FLAGS := -g -I$(INCLUDE) -O0
TARGET := hello

SRC=./src
OBJ=./obj

SOURCES := $(wildcard $(SRC)/*.c)
OBJECTS := $(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SOURCES))

$(TARGET): $(OBJECTS)
	$(LD) -T lpc1114.ld -o $@ $^

$(OBJ)/%.o: $(SRC)/%.c
	$(CC) $(FLAGS) -c $< -o $@

clean:
	rm -f $(INCLUDE)/~*
	rm -f $(SRC)/~*
	rm -f $(OBJ)/*.o

