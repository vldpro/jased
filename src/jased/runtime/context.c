#include <malloc.h>
#include "jased/runtime/context.h"

jased_ctx_t* jased_ctx_new() {
	jased_ctx_t* new_ctx = (jased_ctx_t*)malloc( sizeof(jased_ctx_t) );
	new_ctx-> io_buffer = io_buffer_new();
	
	new_ctx-> is_new_cycle_enable = 0;
	new_ctx-> command_pointer = 0;
	new_ctx-> current_line = 0;
	new_ctx-> commands_count = 0; 

	return new_ctx;
}

void jased_ctx_delete( jased_ctx_t* ctx ) {
	io_buffer_delete( ctx-> io_buffer );
	free(ctx);
}
