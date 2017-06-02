#include <malloc.h>

#include "jased/runtime/interpreter.h"
#include "jased/runtime/context.h"
#include "jased/commands/commands.h"
#include "jased/jased_exit_status.h"
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

static void delete( 
        io_buffer_t* iobuf, 
        string_buffer_t* ps, 
        string_buffer_t* hs,
        string_buffer_t* after,
        string_buffer_t* before ) {

    io_buffer_delete( iobuf );
    sbuffer_delete( ps );
    sbuffer_delete( hs );
    sbuffer_delete( after );
    sbuffer_delete( before );
}

/* interpreter for interpreter_context */
void run( int const in_stream, interpreter_ctx_t** const int_contexts, size_t const contexts_count ) {
	size_t line_num = 1;
    size_t j = 0;
    io_buffer_t* iobuf = io_buffer_new();

    string_buffer_t* pattern_space = sbuffer_new();
    string_buffer_t* hold_space = sbuffer_new();
    string_buffer_t* after_buffer = sbuffer_new();
    string_buffer_t* before_buffer = sbuffer_new();

    if ( contexts_count == 0 ) return;

    for ( j = 0; j < contexts_count; j++ ) {
        jased_ctx_t* const jased_ctx = int_contexts[j]-> jased_ctx;

        jased_ctx-> in_stream = in_stream;
        jased_ctx-> out_stream = STDOUT_FILENO;
        jased_ctx-> io_buffer = iobuf;

        jased_ctx-> hold_space = hold_space;
        jased_ctx-> pattern_space = pattern_space;
        jased_ctx-> after = after_buffer;
        jased_ctx-> before = before_buffer;
    }

	for( ; ; ) {
		ssize_t res = readln( in_stream, iobuf, pattern_space );

        for ( j = 0; j < contexts_count; j++ ) {
            int_contexts[j]-> jased_ctx-> current_line = line_num;
        }

        #ifdef DEBUG_RUNTIME
        printerr("\n============RUNTIME DEBUG INFO=============\n");

        printerr( "-dbg- Current line:" );
        print_int( STDERR_FILENO, line_num );
        printerr("\n");

        printerr("-dbg----------- PATTERN SPACE --------------+\n");
        printerr( pattern_space-> char_at );
        printerr("\n");

        printerr("-dbg------------  HOLD SPACE  --------------+\n");
        printerr( hold_space-> char_at );
        printerr("\n");

        printerr("==================END======================\n");
        #endif

        line_num++;

		if ( res != -1 ) {
            /* interpret all contexts */
            for ( j = 0; j < contexts_count; j++ ) {
                jased_ctx_t* jased_ctx = int_contexts[j]-> jased_ctx;
			    size_t i = 0;

			    jased_ctx-> command_pointer = 
                    jased_ctx-> is_new_cycle_enable = 
                        jased_ctx-> is_any_subs_matched = 0;

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
                        if ( command_exval == UNDEFINED_LABEL_FOR_BRANCH ) {
                            printerr("jased: undefined label for branch.\n");
                            exit(ERROR_RUNTIME);
                        }

                        delete( 
                            iobuf, pattern_space, 
                            hold_space, after_buffer, 
                            before_buffer 
                        );

					    return;
				    }

				    jased_ctx-> command_pointer++;
			    }
            }

            /* if default output enable, on each cycle sed print pattern space */
            if ( int_contexts[0]-> jased_ctx-> is_default_output_enable ) {
                print_ps( int_contexts[0]-> jased_ctx );
            }

		} else {
            delete( 
                iobuf, pattern_space, 
                hold_space, after_buffer, 
                before_buffer 
            );

			return;
		}
	}
}
