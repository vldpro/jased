#include <unistd.h>
#include <stdio.h>

#include "jased/io/io.h"

#define HANDLE_READ_ERR( action ) \
if ( (action) < 1 ) return -1

ssize_t readln( int const stream, io_buffer_t* in, string_buffer_t* const dest ) {
	size_t i;
	sbuffer_clear( dest );

	if ( io_buffer_is_empty(in) ) {
		HANDLE_READ_ERR( io_buffer_read_next(stream, in) );
	}

	for( ; ; ) {
		i = in-> start_idx;	
		for ( ; i <= in-> end_idx; i++ ) {
			if ( in-> buffer[i] == '\n' ) break;
		}

		#ifdef DEBUG_INPUT
		printf( " start: %i end: %i i : %lu \n", in-> start_idx, in-> end_idx,  i );
		#endif
			
		sbuffer_append(
			dest,
			io_buffer_get_start_ptr( in ),
			i == in-> end_idx + 1 ? 
				i - in-> start_idx : i - in-> start_idx + 1 
		);

		in-> start_idx = i + 1;

		if ( i != in-> end_idx + 1 ) break;
		HANDLE_READ_ERR( io_buffer_read_next(stream, in) );
	}

	return 0;
}

ssize_t print( int const stream, string_buffer_t* const string ) {
	return write( stream, string-> char_at, string-> eos );
}

#ifdef DEBUG_INPUT
int main( int argc, char** argv ) {
	string_buffer_t* str = sbuffer_new();
	io_buffer_t* iobuf = io_buffer_new_size(5);	

	while( readln(STDIN_FILENO, iobuf, str) != -1 ) {
		print( STDOUT_FILENO, str );
	}

	return 0;
}
#endif
