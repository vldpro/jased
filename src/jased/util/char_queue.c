#include <stdlib.h>

#include "jased/util/chars_queue.h"

chars_queue_t* cqueue_new( string_buffer_t* buffer ) {
	chars_queue_t* cqueue = (char_queue_t*)malloc( sizeof(char_queue_t) );
	cqueue-> buffer = buffer;
	cqueue-> start = 0;

	return cqueue;
}

char cqueue_getc( chars_queue_t* const cqueue ) {
	return cqueue-> buffer-> char_at[ cqueue-> start++ ];
}

char cqueue_gettop( chars_queue_t* const) {
}

int cqueue_is_empty( chars_queue_t* const cqueue ) {
	return cqueue-> start == cqeue-> buffer -> eos;
}

void cqueue_free( chars_queue_t* cqueue ) {
	free( cqueue );
}
