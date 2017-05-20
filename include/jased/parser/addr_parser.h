#ifndef JASED_ADDR_PARSER
#define JASED_ADDR_PARSER

#include <stddef.h>

enum address_parser_state {
	PARSING_START,
	PARSING_FIRST_ADDRESS,
	PARSING_SECOND_ADDRESS,
	PARSING_END
};

enum parser_status
parse_address( char const* line, size_t const length, enum address_parser_state state);

#endif
