#ifndef JASED_LINE_PARSER
#define JASED_LINE_PARSER

#include "jased/util/chars_queue.h"
#include "jased/runtime/interpreter.h"
#include "jased/parser/parser_context.h"
#include "jased/parser/errors.h"

parser_status_t parse_line(
    chars_queue_t* const cqueue,
    interpreter_ctx_t* const int_ctx,
    parser_ctx_t* const parser_ctx
);

#endif
