#include "jased_context.h"
#include "malloc.h"

jased_ctx_t* jased_ctx_new() {
	jased_ctx_t* new_ctx = (jased_ctx_t*)malloc( sizeof(jased_ctx_t) );
	new_ctx-> pattern_buffer = sbuffer_new();
	new_ctx-> hold_buffer = sbuffer_new();

	new_ctx-> current_line = 1;
	new_ctx-> command_pointer = 0;
	new_ctx-> cmd_queue = cmdqueue_new();

	return new_ctx;
}

void jased_ctx_free( jased_ctx_t* ctx ) {
	sbuffer_delete( ctx-> pattern_buffer );
	sbuffer_delete( ctx-> hold_buffer );
	cmdqueue_free( ctx-> cmd_queue );
	free( ctx );
}
