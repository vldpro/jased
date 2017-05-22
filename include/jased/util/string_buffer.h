#ifndef JASED_STRING_BUFFER
#define JASED_STRING_BUFFER
#include <stdlib.h>

typedef struct string_buffer {
	size_t eos;
	size_t size;

	char* char_at;
} string_buffer_t;

string_buffer_t* sbuffer_new();

string_buffer_t* sbuffer_init( char const* const initial );

void sbuffer_reinit( string_buffer_t* const buffer, char const* const string );

void sbuffer_reinit_part( string_buffer_t* const buffer, char const* const string, size_t const string_len );

void sbuffer_append( string_buffer_t* const buffer, char const* const byte_seq, size_t const length );

void sbuffer_append_char( string_buffer_t* const buffer, char const sym );

void sbuffer_append_buf( string_buffer_t* const dest, string_buffer_t const* const src );

string_buffer_t* sbuffer_clone( string_buffer_t const* const buffer );

string_buffer_t* sbuffer_truncate( string_buffer_t* const buffer );

void sbuffer_clear( string_buffer_t* const buffer );

void sbuffer_delete( string_buffer_t* buffer );

void sbuffer_set_end_of_string( string_buffer_t* const buffer, size_t const i );

int sbuffer_is_empty( string_buffer_t* const buffer);

#endif
