#include <malloc.h>
#include "jased/runtime/context.h"

jased_ctx_t* jased_ctx_new() {
	jased_ctx_t* new_ctx = (jased_ctx_t*)malloc( sizeof(jased_ctx_t) );
	
    new_ctx-> labels = hmap_new( default_hash, 1000 ); 

	new_ctx-> is_new_cycle_enable = 
        new_ctx-> is_last_line =
        new_ctx-> is_any_subs_matched = 
        new_ctx-> command_pointer = 
        new_ctx-> current_line = 
        new_ctx-> commands_count = 0; 

	return new_ctx;
}

void jased_ctx_delete( jased_ctx_t* ctx ) {
    hmap_delete( ctx-> labels );
	free(ctx);
}
