#include <malloc.h>

#include "jased/runtime/interpreter.h"
#include "jased/runtime/context.h"
#include "jased/io/io.h"

interpreter_ctx_t* interpreter_ctx_new() {
	interpreter_ctx_t* int_ctx = (interpreter_ctx_t*)malloc( sizeof(interpreter_ctx_t) );
	int_ctx-> jased_ctx = jased_ctx_new();
	int_ctx-> executors_list = execlist_new();

	return int_ctx;
}

void interpreter_ctx_delete( interpreter_ctx_t* int_ctx ) {
	jased_ctx_delete( int_ctx-> jased_ctx );
	execlist_delete( int_ctx-> executors_list );
	free( int_ctx );
}

static void delete( string_buffer_t* line, io_buffer_t* iobuf ) {
    sbuffer_delete( line );
    io_buffer_delete( iobuf );
}

void run( interpreter_ctx_t* const int_ctx ) {
	size_t line_num = 1;
    io_buffer_t* iobuf = io_buffer_new();

	for( ; ; ) {
		string_buffer_t* line = sbuffer_new(); 
        jased_ctx_t* jased_ctx = int_ctx-> jased_ctx;

		ssize_t res = readln( jased_ctx-> in_stream, iobuf, line );

        /* merge new line with pattern space and delete old ps */
		sbuffer_append_buf( line, jased_ctx-> pattern_space );
		sbuffer_delete( jased_ctx-> pattern_space );

        /* assign new pattern space */
		jased_ctx-> pattern_space = line;
		jased_ctx-> current_line = line_num++;

		if ( res != -1 ) {
			size_t i = 0;
			jased_ctx-> command_pointer = 0;
			jased_ctx-> is_new_cycle_enable = 0;

			while ( (i = jased_ctx-> command_pointer) < jased_ctx-> commands_count ) {
				int command_exval;
				if ( jased_ctx-> is_new_cycle_enable ) 
					break;

				command_exval = int_ctx-> executors_list-> executors[i]-> run(
					int_ctx-> executors_list-> executors[i]
				);

				if ( command_exval != 0 ) {
                    delete( line, iobuf );
					return;
				}

				jased_ctx-> command_pointer++;
			}

		} else {
            delete( line, iobuf );
			return;
		}
	}
}
