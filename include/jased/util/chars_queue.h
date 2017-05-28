#ifndef CHARS_QUEUE
#define CHARS_QUEUE

#include "jased/util/string_buffer.h"

typedef struct chars_queue {
	string_buffer_t* buffer;
	long start;
} chars_queue_t;


chars_queue_t* cqueue_new( string_buffer_t* buffer );

void cqueue_reinit( chars_queue_t* const cqueue, string_buffer_t* buffer );

char cqueue_getc( chars_queue_t* const cqueue );

char cqueue_gettop( chars_queue_t* const cqueue );

int cqueue_is_empty( chars_queue_t* const cqueue );

void cqueue_delete( chars_queue_t* cqueue );

void cqueue_push_back( chars_queue_t* cqueue, char const sym );


#endif
