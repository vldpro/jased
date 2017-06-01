#include "jased/parser/errors.h"
#include "jased/parser/line_parser.h"
#include "jased/parser/parser_context.h"
#include "jased/parser/jased_parser.h"


#include "jased/util/string_buffer.h"
#include "jased/util/chars_queue.h"

#include "jased/runtime/interpreter.h"
#include "jased/runtime/executors.h"

#include "jased/commands/commands.h"


#ifdef DEBUG_PARSING
#include <jased/io/io.h>
#include <unistd.h>
#endif

static void delete( chars_queue_t* cqueue, parser_ctx_t* parser_ctx, string_buffer_t* sbuffer ) {
    cqueue_delete( cqueue );
    /* TODO DELETE */
    sbuffer_clear( sbuffer );
    parser_ctx_delete( parser_ctx );
}

#ifdef DEBUG_PARSING
static void debug_parsing( char* const result, interpreter_ctx_t* const int_ctx ) {
    printerr("Parsing debug info from 'script_parser':\n");

    printerr("\t-dbg- Parsing status: ");
    printerr( result );
    printerr("\n");

    printerr("\t-dbg- Parsed commands_count: "); 
    print_int( STDERR_FILENO, int_ctx-> jased_ctx-> commands_count );
    printerr("\n");

    printerr("\t-dbg- Default output: "); 
    printerr( int_ctx-> jased_ctx-> is_default_output_enable ? "ENABLE" : "DISABLE");
    printerr("\n");

}
#endif

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
        exit_stat.stop_on_symbol = cqueue-> start + 1;
        exit_stat.stopped_on_string = script;
        exit_stat.parser_status = stat == PARSING_OK ? UNCLOSED_CMD_LIST : stat;

        #ifdef DEBUG_PARSING
        debug_parsing("FAILED", int_ctx ); 
        #endif

        delete( cqueue, parser_ctx, script_buf );
        return exit_stat; 
    }

    execlist_set(
        int_ctx-> executors_list,
        int_ctx-> jased_ctx-> commands_count++,
        construct_no_params_executor(
            int_ctx-> jased_ctx, empty_cmd 
        )
    );

    delete( cqueue, parser_ctx, script_buf );


    #ifdef DEBUG_PARSING
    debug_parsing("OK", int_ctx ); 
    #endif

    exit_stat.is_ok = 1;

    return exit_stat; 
}
