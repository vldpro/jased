#ifndef JASED_PARSER
#define JASED_PARSER

#include "jased/runtime/interpreter.h"
#include "jased/parser/errors.h"

typedef struct parser_exit_status {
    int is_ok;
    int stop_on_line;
    int stop_on_symbol;

    parser_status_t parser_status;
} parser_exit_status_t;

parser_exit_status_t parse_file(
        int const file,
        interpreter_ctx_t* const int_ctx
);

parser_exit_status_t parse_commandline_script(
        char* const script,
        interpreter_ctx_t* const int_ctx
);

#endif
