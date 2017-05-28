#ifndef PARSER_CONTEXT
#define PARSER_CONTEXT
#include <stddef.h> 

typedef struct optional {
    /* condition for commands list */
    struct condition* cond; 
    /* pointer for inserting codition after commands list closed */
    size_t cond_command_pointer; 
    /* is condition present for this bracket*/
    int is_present; 
} optional_t;

typedef struct optional_stack {
    optional_t* conditions;
    size_t top, size;
} optional_stack_t;

typedef struct parser_context {
    /* count of unclosed brackets or depth of commands list */
    size_t depth; 
    /* conditions stack */
    optional_stack_t* cond_stack; 
} parser_ctx_t;


optional_stack_t* optstack_new();

void optstack_delete( optional_stack_t* stack );

void optstack_push( optional_stack_t* const stack, optional_t opt );

optional_t optstack_pop( optional_stack_t* const stack );

parser_ctx_t* parser_ctx_new();

void parser_ctx_delete( parser_ctx_t* ctx );

#endif
