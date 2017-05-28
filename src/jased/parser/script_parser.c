#include "jased/parser/errors.h"
#include "jased/parser/line_parser.h"
#include "jased/parser/parser_context.h"
#include "jased/parser/jased_parser.h"


#include "jased/util/string_buffer.h"
#include "jased/util/chars_queue.h"

#include "jased/runtime/interpreter.h"
#include "jased/runtime/executors.h"

#include "jased/commands/commands.h"

static void delete( chars_queue_t* cqueue, parser_ctx_t* parser_ctx, string_buffer_t* sbuffer ) {
    sbuffer_delete( sbuffer );
    cqueue_delete( cqueue );
    parser_ctx_delete( parser_ctx );
}

parser_exit_status_t
parse_commandline_script( char* const script, interpreter_ctx_t* const int_ctx ) {
    string_buffer_t* script_buf = sbuffer_init( script );
    chars_queue_t* cqueue = cqueue_new(script_buf);
    parser_ctx_t* parser_ctx = parser_ctx_new();
    parser_exit_status_t exit_stat;

    parser_status_t stat = parse_line( cqueue, int_ctx, parser_ctx );

    if ( stat != PARSING_OK || parser_ctx-> depth != 0 ) {
        exit_stat.is_ok = 0;
        exit_stat.stop_on_line = 1;
        exit_stat.stop_on_symbol = cqueue-> start;
        exit_stat.parser_status = stat;

        delete( cqueue, parser_ctx, script_buf );
        return exit_stat; 
    }

    if ( int_ctx-> jased_ctx-> is_default_output_enable ) {
        /* if default output enable, on each cycle sed print pattern space */
        execlist_set(
                int_ctx-> executors_list,
                int_ctx-> jased_ctx-> commands_count++,
                construct_no_params_executor(
                    int_ctx-> jased_ctx, print_ps
                )
        );
    }

    /* each cycle sed clean pattern space */
    execlist_set(
        int_ctx-> executors_list,
        int_ctx-> jased_ctx-> commands_count++,
        construct_no_params_executor(
            int_ctx-> jased_ctx, clear_ps 
        )
    );


    delete( cqueue, parser_ctx, script_buf );

    exit_stat.is_ok = 1;

    return exit_stat; 
}
