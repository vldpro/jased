#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "jased/io/io.h"
#include "jased/jased_exit_status.h"


#define HANDLE_READ_ERR( action ) \
    do { \
        int res = (action); \
        int errnum = errno; \
        if ( res == -1 ) { \
            char* const errmsg = strerror(errnum); \
            printerr("jased: read error : "); \
            printerr(errmsg); \
            printerr("\n"); \
            exit(ERROR_IO); \
        } else if ( res == 0 ) return -1; \
    } while(0) 

#define WRITE( stream, object, count ) \
    do { \
        int res = write( (stream), (object), (count) ); \
        if ( res == -1 ) { \
            exit(ERROR_IO); \
        } \
    } while(0)

int open_filem( char* const filename, int const flags, mode_t mode ) {
    int fd = open( filename, flags, mode );
    int errnum = errno;

    if ( fd != -1 ) return fd;

    printerr("jased: can not open file '");
    printerr( filename );
    printerr("' : ");
    printerr( strerror(errnum) );
    printerr("\n");
    exit(ERROR_IO);
}

int open_file( char* const filename, int const flags ) {
    int fd = open( filename, flags );
    int errnum = errno;

    if ( fd != -1 ) return fd;

    printerr("jased: can not open file '");
    printerr( filename );
    printerr("' : ");
    printerr( strerror(errnum) );
    printerr("\n");
    exit(ERROR_IO);
}

int close_file( int const fd ) {
    int result = close(fd);
    int errnum = errno;

    if ( result != -1 ) return result;

    printerr("jased: internal error - can not close file with fd '");
    print_int( STDERR_FILENO,  fd );
    printerr("' : ");
    printerr( strerror(errnum) );
    printerr("\n");
    exit(ERROR_IO);
}

ssize_t cache_line( int const stream, io_buffer_t* const in ) {
    string_buffer_t* cache = sbuffer_new();
    int res = readln( stream, in, cache );

    if ( res == -1 ) {
        sbuffer_delete(cache);
    } else in-> cache = cache;

    return res;
}

ssize_t readln( int const stream, io_buffer_t* in, string_buffer_t* const dest ) {
	size_t i;
	sbuffer_clear( dest );

    if ( in-> cache != NULL ) {
        sbuffer_reinit( dest, in-> cache-> char_at);
        sbuffer_delete(in-> cache);
        in-> cache = NULL;
        return 0;
    }

	if ( io_buffer_is_empty(in) ) {
		HANDLE_READ_ERR( io_buffer_read_next(stream, in) );
	}

	for( ; ; ) {
		i = in-> start_idx;	
		for ( ; i <= in-> end_idx; i++ ) {
			if ( in-> buffer[i] == '\n' ) break;
		}

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
	WRITE( stream, string-> char_at, string-> eos );
    return 0;
}

static ssize_t println_stream( int const stream, char const * const str ) {
    WRITE( stream, str, strlen(str) );
    WRITE( stream, "\n", 1 );

    return 0;
}

ssize_t print_stream( int const stream, char const * const str ) {
    WRITE( stream, str, strlen(str) );
    return 0;
}

ssize_t printerr( char const* const str ) {
    return print_stream( STDERR_FILENO, str );
}

ssize_t println( char const* const str ) {
    return println_stream( STDOUT_FILENO, str );
}

#include <stdio.h>

ssize_t print_int( int const stream, int const value ) {
    char number[15] = {0};

    int nums = sprintf( number, "%d", value );

    WRITE( stream, number, nums );

    return 0;
}
