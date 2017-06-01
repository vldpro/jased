#include <ctype.h>
#include "jased/parser/util.h"

int skip_spaces( chars_queue_t* const cqueue ) {	
	int i = 0;
	while ( !cqueue_is_empty(cqueue) ) {
		char const sym = cqueue_gettop(cqueue);

		if ( isspace(sym) ) {
			i++; cqueue_getc(cqueue);
		} else return i;
	}

	return -1;
}
