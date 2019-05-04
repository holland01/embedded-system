CC := gcc-arm
LD := ld-arm

INCLUDE=./include

FLAGS := -g -mcpu=cortex-m0 -I$(INCLUDE) -O0
TARGET := servo

SRC=./src
OBJ=./obj

SOURCES := $(wildcard $(SRC)/*.c)
OBJECTS := $(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SOURCES))

OPENOCD_ROOT := ../openocd/tcl

OPENOCD_CMD_FLASH := "program $(TARGET) verify reset exit"
OPENOCD_TARGET := $(OPENOCD_ROOT)/target/lpc11xx.cfg
OPENOCD_INTERFACE := $(OPENOCD_ROOT)/interface/stlink.cfg

$(TARGET): $(OBJECTS)
	$(LD) -T lpc1114.ld -o $@ $^

objmake:
	mkdir -p obj

$(OBJ)/%.o: $(SRC)/%.c objmake
	$(CC) $(FLAGS) -c $< -o $@

flash:
	cp $(TARGET) $(OPENOCD_ROOT)/$(TARGET)
	openocd -f $(OPENOCD_TARGET) -f $(OPENOCD_INTERFACE) -c $(OPENOCD_CMD_FLASH) 

clean:
	rm -f $(TARGET)
	rm -f *~
	rm -f $(INCLUDE)/*~
	rm -f $(SRC)/*~
	rm -f $(OBJ)/*.o

