#include "jased/runtime/executors_list.h"
#include <malloc.h>

typedef struct executors_list {
	executor_t* executors;
	size_t size;
	int last_command;
} executors_list_t;

executors_list_t* execlist_new() {
	executors_list_t* new_queue = (executors_list_t*)malloc( sizeof(executors_list_t) );

	new_queue-> size = DEFAULT_CMD_QUEUE_SIZE;
	new_queue-> last_command = -1;
	new_queue-> executors = (executor_t*)malloc( sizeof(executor_t) * new_queue-> size );

	return new_queue;
}

void execlist_free( executors_list_t* execlist) {
	size_t i = 0;
	for ( ; i < size; i++ ) {
		execlist-> executors[i]-> clean(
			execlist-> executors + i
		);
	}

	free( execlist );
}

executor_t* execlist_get( executors_list_t* const execlist, size_t const i ) {
	return execlist-> executors[i]; 
}

static void on_overflow( executors_list_t* const execlist ) {
	execlist-> size += DEFAULT_CMD_QUEUE_SIZE;
	realloc( execlist-> executors, sizeof(executor_t) * execlist-> size );
}

void execlist_push_back( executors_list_t* const execlist, executor_t const* const  executor ) {
	if ( execlist-> last_command == execlist-> size - 1 )  { 
		on_overflow(execlist);	
	}

	execlist-> executors[ ++cmd_queue-> last_command ] = executor;
}

