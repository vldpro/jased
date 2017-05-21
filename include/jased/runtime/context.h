#ifndef JASED_CONTEXT
#define JASED_CONTEXT

#include "jased_string_buffer.h"
#include "jased_commands_queue.h"

typedef struct jased_ctx {
	string_buffer_t* pattern_buffer;
	string_buffer_t* hold_buffer;

	size_t current_line;
	size_t command_pointer;
	commands_queue_t* cmd_queue;

} jased_ctx_t;


jased_ctx_t* jased_ctx_new();

void jased_ctx_free( jased_ctx_t* ctx );

#endif
