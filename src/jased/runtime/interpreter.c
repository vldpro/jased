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

/* interpreter for interpreter_context */
void run( int const in_stream, interpreter_ctx_t** const int_contexts, size_t const contexts_count ) {
	size_t line_num = 1;
    size_t j = 0;
    io_buffer_t* iobuf = io_buffer_new();

    for ( j = 0; j < contexts_count; j++ ) {
        int_contexts[j]-> jased_ctx-> in_stream = in_stream;
        int_contexts[j]-> jased_ctx-> out_stream = STDOUT_FILENO;
    }

	for( ; ; ) {
		string_buffer_t* line = sbuffer_new(); 

		ssize_t res = readln( in_stream, iobuf, line );

        line_num++;
        for ( j = 0; j < contexts_count; j++ ) {
            jased_ctx_t* jased_ctx = int_contexts[j]-> jased_ctx;
            string_buffer_t* new_pattern_space = sbuffer_clone( line );

            /* merge new line with pattern space and delete old ps */
		    sbuffer_append_buf( new_pattern_space, jased_ctx-> pattern_space );
		    sbuffer_delete( jased_ctx-> pattern_space );


            /* assign new pattern space */
		    jased_ctx-> pattern_space = new_pattern_space;
		    jased_ctx-> current_line = line_num;
        }

		if ( res != -1 ) {
            for ( j = 0; j < contexts_count; j++ ) {
                jased_ctx_t* jased_ctx = int_contexts[j]-> jased_ctx;
			    size_t i = 0;
			    jased_ctx-> command_pointer = 0;
			    jased_ctx-> is_new_cycle_enable = 0;

                while ( (i = jased_ctx-> command_pointer) < jased_ctx-> commands_count ) {
			    	int command_exval;
			    	if ( jased_ctx-> is_new_cycle_enable ) 
			    		break;

			    	command_exval = int_contexts[j]-> executors_list-> executors[i]-> run(
			    		int_contexts[j]-> executors_list-> executors[i]
			    	);

    				if ( command_exval != 0 ) {
                        delete( line, iobuf );
					    return;
				    }

				    jased_ctx-> command_pointer++;
			    }
            }

		} else {
            delete( line, iobuf );
			return;
		}
	}
}
