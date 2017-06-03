#ifndef JASED_IO_BUFFER
#define JASED_IO_BUFFER

#define INPUT_BUFFER_SIZE 4096
#include <stdlib.h>
#include <unistd.h>

#include "jased/util/string_buffer.h"

typedef struct io_buffer {
	int start_idx;
	int end_idx;

	char* buffer;
	size_t size;

    string_buffer_t* cache;
} io_buffer_t;

io_buffer_t* io_buffer_new();

io_buffer_t* io_buffer_new_size( size_t size );

int io_buffer_is_empty( io_buffer_t const * const );

char* io_buffer_get_start_ptr( io_buffer_t const * const io_buffer );

void io_buffer_clear( io_buffer_t* const io_buffer );

void io_buffer_delete( io_buffer_t* const io_buffer );

ssize_t io_buffer_read_next( int const stream, io_buffer_t* const io_buffer );


#endif
