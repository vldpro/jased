#include "jased/io/io_buffer.h"

#include <string.h>

io_buffer_t* io_buffer_new() {
	return io_buffer_new_size( INPUT_BUFFER_SIZE );
}

io_buffer_t* io_buffer_new_size( size_t size ) {
	io_buffer_t* new_buffer = (io_buffer_t*)malloc( sizeof(io_buffer_t) );
	new_buffer-> size = size;
	new_buffer-> end_idx = -1;
	new_buffer-> start_idx = -1; 
	new_buffer-> buffer = (char*)malloc( sizeof(char) * size );

	return new_buffer;
}

int io_buffer_is_empty( io_buffer_t const * const io_buffer ) {
	return io_buffer-> end_idx == -1 || io_buffer-> end_idx < io_buffer-> start_idx;
}

char* io_buffer_get_start_ptr( io_buffer_t const * const io_buffer ) {
	return io_buffer-> start_idx != -1 ? 
		io_buffer-> buffer + io_buffer-> start_idx : io_buffer-> buffer
	;
}

void io_buffer_clear( io_buffer_t* const io_buffer ) {
	io_buffer-> start_idx = -1;
	io_buffer-> end_idx = -1;
}

ssize_t io_buffer_read_next( int const stream, io_buffer_t* const io_buffer ) {
	ssize_t real_size = read( 
		stream, 
		io_buffer-> buffer, 
		io_buffer-> size
	);

	if ( real_size > 0 ) { 
		io_buffer-> start_idx = 0;
		io_buffer-> end_idx = real_size - 1;
	}
	
	return real_size;
}
