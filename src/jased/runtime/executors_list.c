#include "jased/runtime/executors_list.h"
#include <malloc.h>

executors_list_t* execlist_new() {
	executors_list_t* execlist = (executors_list_t*)malloc( sizeof(executors_list_t) );

	execlist-> capacity = DEFAULT_CMD_QUEUE_SIZE;
	execlist-> size = 0;
	execlist-> executors = (executor_t**)malloc( sizeof(executor_t*) * execlist-> size );

	return execlist;
}

void execlist_delete( executors_list_t* execlist) {
	size_t i = 0;
	for ( ; i < execlist-> size; i++ ) {
		execlist-> executors[i]-> clean(
			execlist-> executors[i] 
		);
	}

	free( execlist );
}

executor_t* execlist_get( executors_list_t* const execlist, size_t const i ) {
	return execlist-> executors[i]; 
}

static void on_overflow( executors_list_t* const execlist ) {
	execlist-> capacity += DEFAULT_CMD_QUEUE_SIZE;
	execlist-> executors = realloc( execlist-> executors, sizeof(executor_t) * execlist-> capacity );
}

void execlist_set( executors_list_t* const execlist, size_t const i, executor_t* const executor ) {
	if ( i > execlist-> capacity - 1 )  { 
		on_overflow(execlist);	
	}

	execlist-> executors[i] = executor;
	execlist-> size++;
}

void execlist_push_back( executors_list_t* const execlist, executor_t * const  executor ) {
	if ( execlist-> size == execlist-> capacity - 1 )  { 
		on_overflow(execlist);	
	}

	execlist-> executors[ (execlist-> size)++ ] = executor;
}
