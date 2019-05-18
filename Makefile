CC := gcc-arm
LD := ld-arm

INCLUDE=./include

FLAGS := -g -ggdb -mcpu=cortex-m0 -I$(INCLUDE) -O1
TARGET := servo

SRC=./src
OBJ=./obj

SOURCES := $(wildcard $(SRC)/*.c)
OBJECTS := $(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SOURCES))

OPENOCD_ROOT := ../openocd/tcl

OPENOCD_CMD_FLASH := "program $(TARGET) verify reset exit"
OPENOCD_TARGET := $(OPENOCD_ROOT)/target/lpc11xx.cfg
OPENOCD_INTERFACE := $(OPENOCD_ROOT)/interface/stlink.cfg

$(TARGET): $(OBJECTS) obj/irq.o
	$(LD) -M -T lpc1114.ld -o $@ $^ > $@.map

objmake:
	mkdir -p obj

$(OBJ)/irq.o: $(SRC)/irq.s objmake
	$(CC) $(FLAGS) -c $< -o $@

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

