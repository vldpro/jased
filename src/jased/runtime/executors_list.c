#include "jased/runtime/executors_list.h"
#include <malloc.h>

executors_list_t* execlist_new() {
	executors_list_t* execlist = (executors_list_t*)malloc( sizeof(executors_list_t) );

	execlist-> size = DEFAULT_CMD_QUEUE_SIZE;
	execlist-> last_command = -1;
	execlist-> executors = (executor_t**)malloc( sizeof(executor_t*) * execlist-> size );

	return execlist;
}

void execlist_free( executors_list_t* execlist) {
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
	execlist-> size += DEFAULT_CMD_QUEUE_SIZE;
	execlist-> executors = realloc( execlist-> executors, sizeof(executor_t) * execlist-> size );
}

void execlist_push_back( executors_list_t* const execlist, executor_t * const  executor ) {
	if ( execlist-> last_command == execlist-> size - 1 )  { 
		on_overflow(execlist);	
	}

	execlist-> executors[ ++(execlist-> last_command) ] = executor;
}

