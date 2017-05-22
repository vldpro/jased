#include <malloc.h>

#include "jased/runtime/interpreter.h"
#include "jased/io/io.h"

interpreter_ctx_t interpreter_ctx_new() {
	interpreter_ctx_t* int_ctx = (interpreter_ctx_t*)malloc( sizeof(interpreter_ctx_t) );
	int_ctx-> jased_ctx = jased_ctx_new();
	int_ctx-> executrors_list = execlist_new();

	return int_ctx;
}

void interpreter_ctx_delete( interpreter_ctx_t* int_ctx ) {
	jased_ctx_delete( int_ctx-> jased_ctx );
	execlist_delete( int_ctx-> executors_list );
	free( int_ctx );
}

void run( interpreter_ctx_t* const int_ctx ) {
	size_t line_num = 1;

	for( ; ; ) {
		string_buffer_t* line = sbuffer_new(); 

		ssize_t res = readln( 
			int_ctx-> jased_ctx-> stream,
			int_ctx-> jased_ctx-> io_buffer,
			line
		);

		sbuffer_append_buf( line, int_ctx-> jased_ctx-> pattern_space );
		sbuffer_delete( int_ctx-> jased_ctx-> pattern_space );

		int_ctx-> jased_ctx-> pattern_space = line;
		int_ctx-> jased_ctx-> current_line = line_num++;

		if ( res != 0 ) {
			int_ctx-> jased_ctx-> command_pointer = 0;

			size_t i;
			while ( (i = int_ctx-> jased_ctx-> command_pointer) < int_ctx-> jased_ctx-> commands ) {
				if ( int_ctx-> jased_ctx-> is_new_cycle_enable ) 
					break;

				int ret = int_ctx-> executors_list[i]-> exec(
					int_ctx-> executors_list[i]
				);

				if ( ret != 0 ) {
					return;
				}

				int_ctx-> jased_ctx-> command_pointer++;
			}

		} else {
			return;
		}
	}
}
