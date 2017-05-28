#include "jased/runtime/interpreter.h"
#include "jased/runtime/executors.h"

#include "jased/parser/errors.h"
#include "jased/parser/parser_context.h"
#include "jased/parser/line_parser.h"
#include "jased/parser/jased_parser.h"

#include "jased/commands/commands.h"
#include "jased/util/chars_queue.h"
#include "jased/io/io.h"


static void delete( chars_queue_t* cqueue, io_buffer_t* iobuf, string_buffer_t* str, parser_ctx_t* parser_ctx ) {
    cqueue_delete( cqueue );
    sbuffer_delete( str );
    io_buffer_delete( iobuf );
    parser_ctx_delete( parser_ctx );
}

parser_exit_status_t
parse_file( int const file, interpreter_ctx_t* const int_ctx ) {
    io_buffer_t* iobuf = io_buffer_new();
    string_buffer_t* str = sbuffer_new();
    chars_queue_t* cqueue = cqueue_new(str);
    parser_ctx_t* parser_ctx = parser_ctx_new();
    parser_status_t stat = PARSING_OK;
    parser_exit_status_t exit_stat;

    exit_stat.stop_on_line = 0;

    while( readln(file, iobuf, str) != -1 ) {
        exit_stat.stop_on_line++;
        cqueue_reinit( cqueue, str );

        if ( (stat = parse_line(cqueue, int_ctx, parser_ctx)) != PARSING_OK ) {
            exit_stat.is_ok = 0;
            exit_stat.stop_on_symbol = cqueue-> start;
            exit_stat.parser_status = stat;

            delete( cqueue, iobuf, str, parser_ctx );

            return exit_stat;
        }
    }

    if ( parser_ctx-> depth != 0 ) {
        exit_stat.is_ok = 0;
        exit_stat.stop_on_symbol = cqueue-> start;
        exit_stat.parser_status = UNCLOSED_CMD_LIST;

        delete( cqueue, iobuf, str, parser_ctx );
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


    delete( cqueue, iobuf, str, parser_ctx );

    exit_stat.is_ok = 1;
    return exit_stat;
}
