#ifndef JASED_PARSER_UTIL
#define JASED_PARSER_UTIL

#include <stddef.h>
#include "jased/util/chars_queue.h"

int skip_spaces( chars_queue_t* const cqueue );

/*size_t parse_operand( char const* line, size_t const length, char const end_sym);*/

#endif
