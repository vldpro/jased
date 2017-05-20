#include "jased_commands_queue.h"
#include <malloc.h>

typedef struct commands_queue {
	executor_t* commands;
	size_t size;
	int last_command;
} commands_queue_t;

commands_queue_t* cmdqueue_new() {
	commands_queue_t* new_queue = (commands_queue_t*)malloc( sizeof(commands_queue_t) );

	new_queue-> size = DEFAULT_CMD_QUEUE_SIZE;
	new_queue-> last_command = -1;
	new_queue-> commands = (executor_t*)malloc( sizeof(executor_t) * new_queue-> size );

	return new_queue;
}

void cmdqueue_free( commands_queue_t* cmd_queue ) {
	free( cmd_queue-> commands );
	free( cmd_queue );
}

static void on_overflow( commands_queue_t* const cmd_queue ) {
	cmd_queue-> size += DEFAULT_CMD_QUEUE_SIZE;
	realloc( cmd_queue-> commands, sizeof(executor_t) * cmd_queue-> size );
}

void cmdqueue_push_back( commands_queue_t* const cmd_queue, executor_t executor ) {
	if ( last_command == size - 1 )  { 
		on_overflow();	
	}

	cmd_queue[ ++cmd_queue-> last_command ] = executor;
}

