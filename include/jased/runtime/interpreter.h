#ifndef JASED_INTERPRETER
#define JASED_INTERPRETER

#include <stddef.h>

#include "jased/runtime/context.h"
#include "jased/runtime/executors_list.h"

/* this struct contains metadata for interpreter */
typedef struct interpreter_ctx {
    /* sed context */
	jased_ctx_t* jased_ctx;

    /* list of executors which can execute sed commands */
	executors_list_t* executors_list;
	int in_stream;
} interpreter_ctx_t;

interpreter_ctx_t* interpreter_ctx_new();

void interpreter_ctx_delete( interpreter_ctx_t* int_ctx );

void run( int const in_stream, interpreter_ctx_t** const int_ctx, size_t const contexts_count );

#endif
