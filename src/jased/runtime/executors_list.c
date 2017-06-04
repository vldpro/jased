#include <malloc.h>

#include "jased/runtime/executors_list.h"
#include "jased/io/io.h"
#include "jased/jased_exit_status.h"

executors_list_t* execlist_new() {
	executors_list_t* execlist = (executors_list_t*)malloc( sizeof(executors_list_t) );

	execlist-> capacity = DEFAULT_CMD_QUEUE_SIZE;
	execlist-> size = 0;
	execlist-> executors = (executor_t**)calloc( sizeof(executor_t*) , execlist-> capacity );

	return execlist;
}

void execlist_delete( executors_list_t* execlist) {
	size_t i = 0;
	for ( ; i < execlist-> size; i++ ) {
		if( execlist-> executors[i] != NULL ) execlist-> executors[i]-> clean(
			execlist-> executors[i] 
		);
	}

    free( execlist-> executors );
	free( execlist );
}

executor_t* execlist_get( executors_list_t* const execlist, size_t const i ) {
	return execlist-> executors[i]; 
}

static void on_overflow( executors_list_t* const execlist ) {
    executor_t** tmp;
	execlist-> capacity += DEFAULT_CMD_QUEUE_SIZE;
	tmp  = (executor_t**)realloc( execlist-> executors, sizeof(executor_t*) * execlist-> capacity );

    if ( tmp != NULL ) {
        execlist-> executors = tmp;
    } else {
        printerr("jased: out of memory.\n");
        exit(ERROR_OOM);
    }
}

void execlist_set( executors_list_t* const execlist, size_t const i, executor_t* const executor ) {
	if ( i > execlist-> capacity - 1 )  { 
		on_overflow(execlist);	
	}

	execlist-> executors[i] = executor;
	(execlist-> size)++;
}

void execlist_push_back( executors_list_t* const execlist, executor_t * const  executor ) {
	if ( execlist-> size == execlist-> capacity - 1 )  { 
		on_overflow(execlist);	
	}

	execlist-> executors[ (execlist-> size)++ ] = executor;
}
