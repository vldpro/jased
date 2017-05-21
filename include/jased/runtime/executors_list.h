#ifndef JASED_COMMANDS_QUEUE
#define JASED_COMMANDS_QUEUE

#include <stddef.h>
#include "jased/runtime/executors.h"

#define DEFAULT_CMD_QUEUE_SIZE 100

typedef struct executors_list {
	executor_t const** commands;
	size_t size;
	int last_command;
} executors_list_t;

executors_list_t* execlist_new();

void execlist_free( executors_list_t* cmd_queue );

void execlist_push_back( executors_list_t* const cmd_queue, executor_t const* const executor );

executor_t* execlist_get( executor_list_t* const execlist, size_t const i );

#endif
