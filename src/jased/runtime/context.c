#include <malloc.h>
#include "jased/runtime/context.h"

jased_ctx_t* jased_ctx_new( int const in_stream, int const out_stream ) {
	jased_ctx_t* new_ctx = (jased_ctx_t*)malloc( sizeof(jased_ctx_t) );
	new_ctx-> pattern_space = sbuffer_new();
	new_ctx-> hold_space = sbuffer_new();
	new_ctx-> io_buffer = io_buffer_new();
	
	new_ctx-> in_stream = in_stream;
	new_ctx-> out_stream = out_stream;

	new_ctx-> command_pointer = 0;
	new_ctx-> current_line = 0;

	return new_ctx;
}

void jased_ctx_delete( jased_ctx_t* ctx ) {
	sbuffer_delete( ctx-> hold_space );
	sbuffer_delete( ctx-> pattern_space );
	io_buffer_delete( ctx-> io_buffer );
	free(ctx);
}
