#include "jased_parser_util.h"

size_t skip_spaces( char const* string ) {	
	size_t i = 0;
	while ( string[i] != '\0' && is_space(string[i++]) );

	return i - 1;
}

static size_t parse_operand( char const* line, size_t const length, char const end_sym ) {
	size_t i = 0;
	size_t chieldchars_count = 0;
		
	for ( ; i < length; i++ ) {
		if ( line[i] == end_sym && (chieldchars_count == 0 || chieldchars % 2 != 0 ) )
				break;

		chieldchars_count = line[i] == CMD_SHIELD_CHAR ? 
			chieldchars + 1 : 0;
		
	}

	return i;
}

