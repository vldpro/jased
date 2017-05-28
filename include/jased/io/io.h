#ifndef JASED_IO
#define JASED_IO

#include <unistd.h>

#include "jased/util/string_buffer.h"
#include "jased/io/io_buffer.h"

ssize_t readln( int const stream, io_buffer_t* in, string_buffer_t* const dest );                    

ssize_t print( int const stream, string_buffer_t* const string );

ssize_t println( char const* const str );

ssize_t printerr( char const* const str );

ssize_t print_int( int const stream, int const value );

#endif
