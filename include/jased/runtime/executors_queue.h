#ifndef JASED_COMMANDS_QUEUE
#define JASED_COMMANDS_QUEUE

#include "jased_executors.h"

#define DEFAULT_CMD_QUEUE_SIZE 100

typedef struct commands_queue {
	executor_t* commands;
	size_t size;
	int last_command;
} commands_queue_t;

commands_queue_t* cmdqueue_new();

void cmdqueue_free( commands_queue_t* cmd_queue );

void cmdqueue_push_back( commands_queue_t* const cmd_queue, executor_t executor );

#endif
