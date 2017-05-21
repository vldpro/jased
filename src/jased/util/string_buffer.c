#include <malloc.h>
#include <string.h>
#include <jased/util/string_buffer.h>

#define DEFAULT_BUFFER_SIZE 1024

static void on_overflow( string_buffer_t* const buffer, size_t const needed_to_append );

string_buffer_t* sbuffer_new() {
	string_buffer_t* buffer = (string_buffer_t*)malloc( sizeof(string_buffer_t) );
	buffer-> eos = 0;
	buffer-> size = DEFAULT_BUFFER_SIZE;

	buffer-> char_at = malloc( sizeof(char) * (DEFAULT_BUFFER_SIZE) );
	buffer-> char_at[0] = '\0';

	return buffer;
}

string_buffer_t* sbuffer_init( char const* const string ) {
	size_t string_len = strlen(string);
	string_buffer_t* buffer = sbuffer_new();

	if ( buffer-> eos + string_len + 1 > buffer-> size ) {
		on_overflow( buffer, string_len );
	}

	strcpy( buffer-> char_at, string );
	buffer-> eos = string_len;
	return buffer;
}

int sbuffer_is_empty( string_buffer_t* const buffer ) {
	return buffer-> char_at[ buffer-> eos ] == '\0'; 
}

void sbuffer_reinit( string_buffer_t* const buffer, char const* const string ) {
	sbuffer_reinit_part( buffer, string, strlen( string ) - 1 );
}

void sbuffer_reinit_part( string_buffer_t* const buffer, char const* const string, size_t const string_len ) {
	if ( buffer-> eos + string_len + 1 > buffer-> size ) {
		on_overflow( buffer, string_len );
	}

	strncpy( buffer-> char_at, string, string_len );
	buffer-> eos = string_len;
}

static void on_overflow( string_buffer_t* const buffer, size_t const needed_to_append ) {
	size_t needed = (buffer-> eos + needed_to_append) / DEFAULT_BUFFER_SIZE + 1;
	char* old_ptr = buffer-> char_at;

	buffer-> char_at = (char*)realloc( buffer-> char_at, needed * DEFAULT_BUFFER_SIZE * sizeof(char) );
	if ( old_ptr != buffer-> char_at ) free( old_ptr );
}

void sbuffer_append( string_buffer_t* const buffer, char const* const byte_seq, size_t const length ) {
	if ( buffer-> eos + length + 1 > buffer-> size ) {
		on_overflow( buffer, length );
	}

	strncat( buffer-> char_at + buffer-> eos, byte_seq, length );

	buffer-> eos += length;
	buffer-> char_at[ buffer-> eos ] = '\0';
}

void sbuffer_append_buf( string_buffer_t* const dest, string_buffer_t const* const src ) {
	sbuffer_append( dest, src-> char_at, src-> eos );
}

void sbuffer_set_end_of_string( string_buffer_t* buffer, size_t const i ) {
	buffer-> char_at[i] = '\0';
	buffer-> eos = i;
}

string_buffer_t* sbuffer_clone( string_buffer_t const* const buffer) {
	return sbuffer_init( buffer-> char_at );
}

void sbuffer_clear( string_buffer_t* buffer ) {
	sbuffer_set_end_of_string( buffer, 0 );
}

void sbuffer_delete( string_buffer_t* buffer ) {
	free( buffer-> char_at );
	free( buffer );
}
