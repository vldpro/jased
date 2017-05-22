#ifndef JASED_CMD_PARSER
#define JASED_CMD_PARSER

#include "jased/parser/addr_parser.h"

struct optional {
	struct address* addr;
	int position;
	int is_present = 0;
};

struct address_stack {
	struct optional addrs;	
	int top = -1;
	size_t size;
};

typedef struct parser_context {
	size_t depth = 0; 
	struct address_stack* addr_stack;
} parser_context_t;

struct address_stack* addrstack_new();

void addrstack_push( struct address_stack* const stack, struct optional );

struct address addrstack_pop( struct address_stack* const stack );

void addrstack_delete( struct address_stack* stack );

struct parser_status parse(  
	int const stream,
	int io_buffer_t* const io_buffer,
	interpreter_ctx_t* const int_ctx,
	struct parser_context* const parser_ctx
);

#endif
