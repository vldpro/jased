#include <malloc.h>
#include "jased/parser/cmd_parser.h"

struct address_stack* addrstack_new() {
	struct address_stack* stack = malloc( sizeof(struct address_stack) );
	stack-> addrs = malloc( sizeof(struct optional) * DEFAULT_ADDRSTACK_SIZE );
	stack-> depth = 0;
	stack-> size = DEFAULT_ADDRSTACK_SIZE;
	return stack;
}

static void on_overflow( struct address_stack* const stack, size_t const new_size ) {
	stack-> addrs = realloc( stack-> addrs, new_size * sizeof(struct optional) );
}

void addrstack_push( struct address_stack* const stack, struct optional addr ) {
	if ( stack-> top + 2 > stack-> size )
		on_overflow( stack, stack-> size + DEFAULT_ADDRSTACK_SIZE );

	stack-> addrs[ ++(stack-> top) ] = addr;
}

struct address addrstack_pop( struct address_stack* const stack ) {
	return stack-> addrs[ stack-> top-- ];
}

void addrstack_delete( struct address_stack* stack ) {
	free( stack-> addrs );
	free( stack );
}
