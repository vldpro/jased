#ifndef JASED_CONTEXT
#define JASED_CONTEXT

#include <stddef.h> 
#include "jased/util/string_buffer.h"

typedef struct jased_ctx {
	string_buffer_t* pattern_buffer;
	string_buffer_t* hold_buffer;

	size_t current_line;
	size_t command_pointer;
} jased_ctx_t;


jased_ctx_t* jased_ctx_new();

void jased_ctx_delete( jased_ctx_t* ctx );

#endif
