CC=gcc

CFLAGS=-std=c89 -ansi -Wall -Werror -pedantic
OPTFLAGS=-march=native -m64 -O2 -finline-functions -ftree-vectorize -pipe
DBGFLAGS=-march=native -pg -m64

INCLUDE= -I ./include/
DEFINE=

# project root paths
SRC_PATH=src/jased
BUILD_PATH=bin
OBJ_PATH=obj
TEST_PATH=src/tests/
TEST_BUILD_PATH=src/tests/build

# modules paths
UTIL_PATH=$(SRC_PATH)/util
IO_PATH=$(SRC_PATH)/io
RUNTIME_PATH=$(SRC_PATH)/runtime
CMDLINE_PATH=$(SRC_PATH)/command_line
PARSER_PATH=$(SRC_PATH)/parser
COMMANDS_PATH=$(SRC_PATH)/commands

all: $(BUILD_PATH)/jased
	@echo PROJECT BUILDING SUCCESS

$(BUILD_PATH)/jased: $(OBJ_PATH)/regex.o $(OBJ_PATH)/commands.o $(OBJ_PATH)/condition_parser.o $(OBJ_PATH)/file_parser.o \
	$(OBJ_PATH)/util.o $(OBJ_PATH)/line_parser.o $(OBJ_PATH)/parser_context.o $(OBJ_PATH)/errors.o	$(OBJ_PATH)/script_parser.o \
	$(OBJ_PATH)/hashmap.o $(OBJ_PATH)/io_buffer.o $(OBJ_PATH)/buffered_io.o $(OBJ_PATH)/main.o $(OBJ_PATH)/executors_constructors.o \
	$(OBJ_PATH)/interpreter.o $(OBJ_PATH)/executors.o $(OBJ_PATH)/executors_list.o $(OBJ_PATH)/context.o $(OBJ_PATH)/string_buffer.o \
	$(OBJ_PATH)/chars_queue.o $(OBJ_PATH)/cmd_line_ctx.o
	
	$(CC) $(DEFINE) $(INCLUDE) $(CFLAGS) $(DBGFLAGS) $(OBJ_PATH)/*.o -o $(BUILD_PATH)/jased

clean:
	rm -r $(OBJ_PATH)/*.o
	@echo REMOVED ALL OBJS

install: all
	install $(BUILD_PATH)/jased $(HOME)/bin

uninstall:
	rm $(HOME)/bin/jased 

# tests
check_all: sbuffer_test cqueue_test sbuffer_check cqueue_check 

sbuffer_test:
	$(CC) $(INCLUDE)  $(CFLAGS) $(TEST_PATH)/sbuffer_test.c $(SRC_PATH)/util/string_buffer.c -o $(TEST_BUILD_PATH)/sbuffer_test

sbuffer_check: sbuffer_test
	./src/tests/build/sbuffer_test

cqueue_test:
	$(CC) $(INCLUDE)  $(CFLAGS) $(TEST_PATH)/cqueue_test.c $(SRC_PATH)/util/*.c -o $(TEST_BUILD_PATH)/cqueue_test

cqueue_check: cqueue_test
	./src/tests/build/cqueue_test

#modules

$(OBJ_PATH)/regex.o: $(COMMANDS_PATH)/regex.c
	$(CC) -c $(CFLAGS) $(OPTFLAGS) $(INCLUDE) $(COMMANDS_PATH)/regex.c -o $(OBJ_PATH)/regex.o

$(OBJ_PATH)/commands.o: $(COMMANDS_PATH)/commands.c
	$(CC) -c $(CFLAGS) $(OPTFLAGS) $(INCLUDE) $(COMMANDS_PATH)/commands.c -o $(OBJ_PATH)/commands.o

$(OBJ_PATH)/condition_parser.o: $(PARSER_PATH)/condition_parser.c
	$(CC) -c $(CFLAGS) $(OPTFLAGS) $(INCLUDE) $(PARSER_PATH)/condition_parser.c -o $(OBJ_PATH)/condition_parser.o

$(OBJ_PATH)/file_parser.o: $(PARSER_PATH)/file_parser.c
	$(CC) -c $(CFLAGS) $(OPTFLAGS) $(INCLUDE) $(PARSER_PATH)/file_parser.c -o $(OBJ_PATH)/file_parser.o

$(OBJ_PATH)/util.o: $(PARSER_PATH)/util.c
	$(CC) -c $(CFLAGS) $(OPTFLAGS) $(INCLUDE) $(PARSER_PATH)/util.c -o $(OBJ_PATH)/util.o

$(OBJ_PATH)/line_parser.o: $(PARSER_PATH)/line_parser.c
	$(CC) -c $(CFLAGS) $(OPTFLAGS) $(INCLUDE) $(PARSER_PATH)/line_parser.c -o $(OBJ_PATH)/line_parser.o

$(OBJ_PATH)/parser_context.o: $(PARSER_PATH)/parser_context.c
	$(CC) -c $(CFLAGS) $(OPTFLAGS) $(INCLUDE) $(PARSER_PATH)/parser_context.c -o $(OBJ_PATH)/parser_context.o

$(OBJ_PATH)/errors.o: $(PARSER_PATH)/errors.c
	$(CC) -c $(CFLAGS) $(OPTFLAGS) $(INCLUDE) $(PARSER_PATH)/errors.c -o $(OBJ_PATH)/errors.o

$(OBJ_PATH)/script_parser.o: $(PARSER_PATH)/script_parser.c
	$(CC) -c $(CFLAGS) $(OPTFLAGS) $(INCLUDE) $(PARSER_PATH)/script_parser.c -o $(OBJ_PATH)/script_parser.o

$(OBJ_PATH)/cmd_line_ctx.o: $(CMDLINE_PATH)/cmd_line_ctx.c
	$(CC) -c $(CFLAGS) $(OPTFLAGS) $(INCLUDE) $(CMDLINE_PATH)/cmd_line_ctx.c -o $(OBJ_PATH)/cmd_line_ctx.o

$(OBJ_PATH)/string_buffer.o: $(UTIL_PATH)/string_buffer.c
	$(CC) -c $(CFLAGS) $(OPTFLAGS) $(INCLUDE) $(UTIL_PATH)/string_buffer.c -o $(OBJ_PATH)/string_buffer.o

$(OBJ_PATH)/chars_queue.o: $(UTIL_PATH)/chars_queue.c
	$(CC) -c $(CFLAGS) $(OPTFLAGS) $(INCLUDE) $(UTIL_PATH)/chars_queue.c -o $(OBJ_PATH)/chars_queue.o

$(OBJ_PATH)/hashmap.o: $(UTIL_PATH)/hashmap.c
	$(CC) -c $(CFLAGS) $(OPTFLAGS) $(INCLUDE) $(UTIL_PATH)/hashmap.c -o $(OBJ_PATH)/hashmap.o

$(OBJ_PATH)/io_buffer.o: $(IO_PATH)/io_buffer.c
	$(CC) -c $(CFLAGS) $(OPTFLAGS) $(INCLUDE) $(IO_PATH)/io_buffer.c -o $(OBJ_PATH)/io_buffer.o

$(OBJ_PATH)/buffered_io.o: $(IO_PATH)/buffered_io.c
	$(CC) -c $(CFLAGS) $(OPTFLAGS) $(INCLUDE) $(IO_PATH)/buffered_io.c -o $(OBJ_PATH)/buffered_io.o

$(OBJ_PATH)/main.o: $(SRC_PATH)/main.c
	$(CC) -c $(CFLAGS) $(OPTFLAGS) $(INCLUDE) $(SRC_PATH)/main.c -o $(OBJ_PATH)/main.o

$(OBJ_PATH)/executors_constructors.o: $(RUNTIME_PATH)/executors_constructors.c
	$(CC) -c $(CFLAGS) $(OPTFLAGS) $(INCLUDE) $(RUNTIME_PATH)/executors_constructors.c -o $(OBJ_PATH)/executors_constructors.o

$(OBJ_PATH)/interpreter.o: $(RUNTIME_PATH)/interpreter.c
	$(CC) -c $(CFLAGS) $(OPTFLAGS) $(INCLUDE) $(RUNTIME_PATH)/interpreter.c -o $(OBJ_PATH)/interpreter.o

$(OBJ_PATH)/executors_list.o: $(RUNTIME_PATH)/executors_list.c
	$(CC) -c $(CFLAGS) $(OPTFLAGS) $(INCLUDE) $(RUNTIME_PATH)/executors_list.c -o $(OBJ_PATH)/executors_list.o

$(OBJ_PATH)/executors.o: $(RUNTIME_PATH)/executors.c
	$(CC) -c $(CFLAGS) $(OPTFLAGS) $(INCLUDE) $(RUNTIME_PATH)/executors.c -o $(OBJ_PATH)/executors.o

$(OBJ_PATH)/context.o: $(RUNTIME_PATH)/context.c
	$(CC) -c $(CFLAGS) $(OPTFLAGS) $(INCLUDE) $(RUNTIME_PATH)/context.c -o $(OBJ_PATH)/context.o

