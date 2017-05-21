#include "jased/runtime/context.h"

typedef struct interpreter_ctx {
	jased_ctx_t* jased_ctx;
	executors_list_t* executors_list;
} interpreter_ctx_t;

interpreter_ctx_t* interpreter_ctx_new();
void interpreter_ctx_delete( interpreter_ctx_t* int_ctx );
