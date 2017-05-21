#ifndef JASED_CONTEXT
#define JASED_CONTEXT

#include <stddef.h> 
#include "jased/util/string_buffer.h"
#include "jased/io/io.h"

typedef struct jased_ctx {
	string_buffer_t* pattern_space;
	string_buffer_t* hold_space;

	size_t current_line;
	size_t command_pointer;
	int in_stream;
	int out_stream;

	io_buffer_t* io_buffer;
} jased_ctx_t;


jased_ctx_t* jased_ctx_new();

void jased_ctx_delete( jased_ctx_t* ctx );

#endif
