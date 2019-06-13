CC := gcc-arm
LD := ld-arm

INCLUDE=./include

LIBS=-L./lib -l:libgcc.a
CCLIBS=#./lib/libgcc.a

FLAGS := -g -ggdb -mcpu=cortex-m0 -I$(INCLUDE) -O0
TARGET := lpc1114image.bin

SRC=./src
OBJ=./obj

SOURCES := $(wildcard $(SRC)/*.c)
OBJECTS := $(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SOURCES))

GCCOBJS := obj/lib/floatunsidf.o \
	obj/lib/_clzsi2.o \
	obj/lib/muldf3.o \
	obj/lib/divdf3.o \
	obj/lib/_udivsi3.o \
	obj/lib/_dvmd_tls.o \
	obj/lib/subdf3.o \
	obj/lib/adddf3.o \
	obj/lib/_fixunsdfsi.o \
	obj/lib/fixdfsi.o \
	obj/lib/_arm_cmpdf2.o \
	obj/lib/ledf2.o \
	obj/lib/eqdf2.o \
	obj/lib/gedf2.o

OPENOCD_ROOT := ../openocd/tcl

OPENOCD_CMD_FLASH := "program $(TARGET) verify reset exit"
OPENOCD_TARGET := $(OPENOCD_ROOT)/target/lpc11xx.cfg
OPENOCD_INTERFACE := $(OPENOCD_ROOT)/interface/stlink.cfg

$(TARGET): $(OBJECTS) obj/irq.o $(GCCOBJS)
	$(LD) -M -T lpc1114.ld $(LIBS) -o $@ $^ > $@.map

objmake:
	mkdir -p obj/lib

$(OBJ)/irq.o: $(SRC)/irq.s objmake
	$(CC) $(CCLIBS) $(FLAGS) -c $< -o $@

$(OBJ)/%.o: $(SRC)/%.c objmake
	$(CC) $(CCLIBS) $(FLAGS) -c $< -o $@

flash:
	cp $(TARGET) $(OPENOCD_ROOT)/$(TARGET)
	openocd -f $(OPENOCD_TARGET) -f $(OPENOCD_INTERFACE) -c $(OPENOCD_CMD_FLASH) 

clean:
	rm -f $(TARGET)
	rm -f *~
	rm -f $(INCLUDE)/*~
	rm -f $(SRC)/*~
	rm -f $(OBJ)/*.o

