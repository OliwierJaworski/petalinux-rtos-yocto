TARGET = vdma_control
# commented out CC because it was using wrong sysroot correc is:--sysroot=/home/--path-to-sdk-sysroot--/sysroots/cortexa9t2hf-neon-xilinx-linux-gnueabi
# because of this it could not find SDL2
#CC = arm-linux-gnueabihf-gcc
CFLAGS = -g -Wall -std=c17
HEADERS_INCLUDE = -Iinclude
CFLAGS += $(HEADERS_INCLUDE)
LDFLAGS = -L/home/Oliwier/Workspace/vdma_sdk/sysroots/cortexa9t2hf-neon-xilinx-linux-gnueabi/usr/lib
LIBS = -lm -lSDL2  

.PHONY: default all clean

default: $(TARGET)
all: default

SRC_DIR = src
SOURCES = $(wildcard $(SRC_DIR)/*c)

HEADER_DIR = include

OBJ_DIR = obj
OBJECTS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SOURCES))
HEADERS = $(wildcard *.h $(HEADER_DIR)/*.h)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

.PRECIOUS: $(TARGET) $(OBJECTS)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -Wall $(LIBS) -o $@

clean:
	-rm -f $(OBJ_DIR)/*.o
	-rm -f $(TARGET)
