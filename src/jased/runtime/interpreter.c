#include <malloc.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

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
        string_buffer_t* print ) {

    io_buffer_delete( iobuf );
    sbuffer_delete( ps );
    sbuffer_delete( hs );
    sbuffer_delete( after );
    sbuffer_delete( print);
}

static io_buffer_t* get_optimal_io_buffer( int const stream ) {
    struct stat stat;
    fstat( stream, &stat );
    return io_buffer_new( stat.st_blksize );
}

/* interpreter for interpreter_context */
void run( int const in_stream, interpreter_ctx_t** const int_contexts, size_t const contexts_count ) {
	size_t line_num = 1;
    size_t j = 0;
    io_buffer_t* iobuf = get_optimal_io_buffer(in_stream); 
    int is_tty = isatty(in_stream);

    string_buffer_t* pattern_space  = sbuffer_new();
    string_buffer_t* hold_space     = sbuffer_new();
    string_buffer_t* after_buffer   = sbuffer_new();
    string_buffer_t* print_buffer   = sbuffer_new();

    if ( contexts_count == 0 ) return;

    for ( j = 0; j < contexts_count; j++ ) {
        jased_ctx_t* const jased_ctx = int_contexts[j]-> jased_ctx;

        jased_ctx-> in_stream = in_stream;
        jased_ctx-> out_stream = STDOUT_FILENO;
        jased_ctx-> io_buffer = iobuf;

        jased_ctx-> hold_space = hold_space;
        jased_ctx-> pattern_space = pattern_space;

        jased_ctx-> print_buffer = print_buffer;
        jased_ctx-> after = after_buffer;
    }

	for( ; ; ) {
		ssize_t res = readln( in_stream, iobuf, pattern_space );
        /* if in_stream is a tty - we can not check last string */
        size_t is_last_line = is_tty ? 0 : (cache_line( in_stream, iobuf ) == -1 ? 1 : 0);


        for ( j = 0; j < contexts_count; j++ ) {
            int_contexts[j]-> jased_ctx-> current_line = line_num;
            int_contexts[j]-> jased_ctx-> is_last_line = is_last_line;
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
                        printerr("FUCK! Internal error: empty executor.\n");
                        exit(ERROR_RUNTIME);
                    }

			    	command_exval = int_contexts[j]-> executors_list-> executors[i]-> run(
			    		int_contexts[j]-> executors_list-> executors[i]
			    	);

    				if ( command_exval != 0 ) {
                        if ( command_exval == UNDEFINED_LABEL_FOR_BRANCH ) {
                            printerr("jased: undefined label for branch.\n");
                            exit(ERROR_RUNTIME);

                        }

                        print( STDOUT_FILENO, print_buffer );

                        if ( int_contexts[0]-> jased_ctx-> is_default_output_enable ) {
                            print( STDOUT_FILENO, pattern_space );
                            /*print_ps( int_contexts[0]-> jased_ctx ); */
                        }

                        print( STDOUT_FILENO, after_buffer );

                        delete( 
                            iobuf, pattern_space, 
                            hold_space, after_buffer, 
                            print_buffer
                        );

					    return;
				    }

				    jased_ctx-> command_pointer++;
			    }
            }

            /* if default output enable, on each cycle sed print pattern space */
            print( STDOUT_FILENO, print_buffer );

            if ( int_contexts[0]-> jased_ctx-> is_default_output_enable ) {
                print( STDOUT_FILENO, pattern_space );
                /*print_ps( int_contexts[0]-> jased_ctx ); */
            }

            print( STDOUT_FILENO, after_buffer );

            sbuffer_clear( print_buffer );
            sbuffer_clear( after_buffer );

		} else {
            delete( 
                iobuf, pattern_space, 
                hold_space, after_buffer, 
                print_buffer
            );

			return;
		}
	}
}
