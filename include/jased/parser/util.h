#ifndef JASED_PARSER_UTIL
#define JASED_PARSER_UTIL

#include <stddef.h>

size_t skip_spaces( char const* string );

size_t parse_operand( char const* line, size_t const length, char const end_sym);

#endif
