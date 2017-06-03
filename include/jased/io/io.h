#ifndef JASED_IO
#define JASED_IO

#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "jased/util/string_buffer.h"
#include "jased/io/io_buffer.h"

ssize_t cache_line( int const stream, io_buffer_t* const in );

ssize_t readln( int const stream, io_buffer_t* in, string_buffer_t* const dest );                    

ssize_t print( int const stream, string_buffer_t* const string );

ssize_t println( char const* const str );

ssize_t print_stream( int const stream, char const* const string );

ssize_t printerr( char const* const str );

ssize_t print_int( int const stream, int const value );

int open_filem( char* const filename, int const flags, mode_t mode );

int open_file( char* const filename, int const flags );

int close_file( int const fd );

#endif
