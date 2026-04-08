TARGET = hello_sdl2

CFLAGS = -g -Wall -std=c17 
HEADERS_INCLUDE = -Iinclude
CFLAGS += -DVERBOSITY=LOG_DEBUG $(HEADERS_INCLUDE)
LDFLAGS = -L/home/Oliwier/Workspace/vdma_sdk/sysroots/cortexa9t2hf-neon-xilinx-linux-gnueabi/usr/lib
LIBS = -lm -lSDL2  

.PHONY: default all clean

default: $(TARGET)
all: default

SRC_DIR = src
SOURCES = $(wildcard $(SRC_DIR)/*.c)

HEADER_DIR = include

OBJ_DIR = build
OBJECTS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SOURCES))
HEADERS = $(wildcard *.h $(HEADER_DIR)/*.h)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(HEADERS)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

.PRECIOUS: $(TARGET) $(OBJECTS)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -Wall $(LIBS) -o $(OBJ_DIR)/$@
	-rm -rf $(SRC_DIR)/bin

clean:
	-rm -rf $(OBJ_DIR)
