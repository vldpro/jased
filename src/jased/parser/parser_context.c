#include <malloc.h>

#include "jased/parser/parser_context.h"

#define OPTSTACK_DEFAULT_SIZE 20

optional_stack_t* optstack_new() {
    optional_stack_t* new_stack = malloc( sizeof(optional_stack_t) );
    new_stack-> conditions = malloc( sizeof(optional_t) * OPTSTACK_DEFAULT_SIZE );
    new_stack-> top = 0;
    new_stack-> size = OPTSTACK_DEFAULT_SIZE;
     
    return new_stack;
}

void optstack_delete( optional_stack_t* stack ) {
    free( stack-> conditions );
    free( stack );
}

void optstack_push( optional_stack_t* const stack, optional_t const opt ) {
    if ( stack-> size == stack-> top ) {
        stack-> size += OPTSTACK_DEFAULT_SIZE;
        stack-> conditions = 
            realloc( stack-> conditions, sizeof(optional_t) * stack-> size ); 

    }

    stack-> conditions[ stack-> top++ ] = opt;
}

optional_t optstack_pop( optional_stack_t* const stack ) {
    return stack-> conditions[--(stack-> top)];
}


parser_ctx_t* parser_ctx_new() {
    parser_ctx_t* new_ctx = malloc( sizeof(parser_ctx_t) );
    new_ctx-> cond_stack = optstack_new();
    new_ctx-> depth = 0;
    return new_ctx;
}

void parser_ctx_delete( parser_ctx_t* ctx ) {
    optstack_delete( ctx-> cond_stack );
    free(ctx);
}
