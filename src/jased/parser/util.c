#include "jased/parser/util.h"

#define IS_SPACE( sym ) \
	sym == ' ' || sym == '\t'

int skip_spaces( chars_queue_t* const cqueue ) {	
	int i = 0;
	while ( !cqueue_is_empty(cqueue) ) {
		char const sym = cqueue_gettop(cqueue);

		if ( IS_SPACE(sym) ) {
			i++; cqueue_getc(cqueue);
		} else return i;
	}

	return -1;
}

/*static size_t parse_operand( char const* line, size_t const length, char const end_sym ) {
	size_t i = 0;
	size_t chieldchars_count = 0;
		
	for ( ; i < length; i++ ) {
		if ( line[i] == end_sym && (chieldchars_count == 0 || chieldchars % 2 != 0 ) )
				break;

		chieldchars_count = line[i] == CMD_SHIELD_CHAR ? 
			chieldchars + 1 : 0;
		
	}

	return i;
}*/

