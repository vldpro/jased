CC=gcc
INCLUDE=-I./include/
CFLAGS=-g -std=c89 -ansi -Wall -Werror -pedantic -m64 
SRC_PATH=src
BIN_PATH=bin
OBJ_PATH=obj

MODULES=jased/util jased/commands jased/command_line jased/io jased/parser jased/runtime jased
MODULES_PATHS=$(addprefix ./$(SRC_PATH)/, $(MODULES))
MODULES_SOURCES=$(wildcard $(addsuffix /*.c,$(MODULES_PATHS)))

MODULES_OBJECTS=$(addprefix $(OBJ_PATH)/, \
	$(shell ls -lR $(SRC_PATH)/jased | sed -n '/.*\.c/ { s@.* \(.*\)\.c@\1.o@gp; }' ) \
)

BUILD_TARGET=$(BIN_PATH)/jased
PRE_BUILD=create_obj_dir
INSTALL_PATH=$(HOME)/bin

vpath %.c $(MODULES_PATHS)

all: $(PRE_BUILD) $(BUILD_TARGET) 

$(PRE_BUILD):; mkdir -p $(OBJ_PATH)
	
$(BUILD_TARGET) : $(MODULES_OBJECTS)
	$(CC) $(CFLAGS) $^ -o $@ 

$(OBJ_PATH)/%.o: %.c
	$(CC) -c $(CFLAGS) $(INCLUDE) $< -o $@

clean:
	rm -r $(OBJ_PATH)
	rm $(BUILD_TARGET)

install: all 
	install $(BUILD_TARGET) $(INSTALL_PATH)

uninstall:
	rm $(INSTALL_PATH)/jased

.PHONY: clean install uninstall $
