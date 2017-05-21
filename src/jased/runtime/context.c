#include <malloc.h>
#include "jased/runtime/context.h"

jased_ctx_t* jased_ctx_new() {
	return (jased_ctx_t*)malloc( sizeof(jased_ctx_t) );
}

void jased_ctx_delete( jased_ctx_t* ctx ) {
	free(ctx);
}
