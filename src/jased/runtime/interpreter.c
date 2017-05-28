#include <malloc.h>

#include "jased/runtime/interpreter.h"
#include "jased/runtime/context.h"
#include "jased/commands/commands.h"
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

static void delete( io_buffer_t* iobuf, string_buffer_t* ps, string_buffer_t* hs ) {
    io_buffer_delete( iobuf );
    sbuffer_delete( ps );
    sbuffer_delete( hs );
}

/* interpreter for interpreter_context */
void run( int const in_stream, interpreter_ctx_t** const int_contexts, size_t const contexts_count ) {
	size_t line_num = 1;
    size_t j = 0;
    io_buffer_t* iobuf = io_buffer_new();
    string_buffer_t* pattern_space = sbuffer_new();
    string_buffer_t* hold_space = sbuffer_new();

    if ( contexts_count == 0 ) return;

    for ( j = 0; j < contexts_count; j++ ) {
        int_contexts[j]-> jased_ctx-> in_stream = in_stream;
        int_contexts[j]-> jased_ctx-> out_stream = STDOUT_FILENO;

        int_contexts[j]-> jased_ctx-> hold_space = hold_space;
        int_contexts[j]-> jased_ctx-> pattern_space = pattern_space;
    }

	for( ; ; ) {
		ssize_t res = readln( in_stream, iobuf, pattern_space );

        for ( j = 0; j < contexts_count; j++ ) {
            int_contexts[j]-> jased_ctx-> current_line = line_num;
        }

        line_num++;

		if ( res != -1 ) {
            /* interpret all contexts */
            for ( j = 0; j < contexts_count; j++ ) {
                jased_ctx_t* jased_ctx = int_contexts[j]-> jased_ctx;
			    size_t i = 0;

			    jased_ctx-> command_pointer = 0;
			    jased_ctx-> is_new_cycle_enable = 0;

                while ( (i = jased_ctx-> command_pointer) < jased_ctx-> commands_count ) {
			    	int command_exval;
			    	if ( jased_ctx-> is_new_cycle_enable ) 
			    		break;

                    if ( int_contexts[j]-> executors_list-> executors[i] == NULL ) {
                        printerr("Internal error: empty executor.\n");
                        exit(5);
                    }

			    	command_exval = int_contexts[j]-> executors_list-> executors[i]-> run(
			    		int_contexts[j]-> executors_list-> executors[i]
			    	);

    				if ( command_exval != 0 ) {
                        delete( iobuf, pattern_space, hold_space );
					    return;
				    }

				    jased_ctx-> command_pointer++;
			    }

                

            }

            /* if default output enable, on each cycle sed print pattern space */
            if ( int_contexts[0]-> jased_ctx-> is_default_output_enable ) {
                print_ps( int_contexts[0]-> jased_ctx );
            }

            /* each cycle sed clean pattern space */
            clear_ps( int_contexts[0]-> jased_ctx );
		} else {
            delete( iobuf, pattern_space, hold_space );
			return;
		}
	}
}
