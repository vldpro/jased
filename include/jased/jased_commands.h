#ifndef JASED_COMMANDS
#define JASED_COMMANDS

#include "jased_string_buffer.h"
#include "stdint.h"
#include "stdlib.h"

#define RT_ERR void

union exec_ctx {
	string_buffer_t* string;
	uint64_t label;
} exec_ctx_t;

typedef RT_ERR (*) ( jased_ctx_t* const, exec_ctx_t const ) cmd_ptr_t;

typedef struct command {
	exec_ctx_t cmd_ctx;	
	cmd_ptr_t ptr;
} command_t;


typedef union command {
	RT_ERR (*no_params) ( jased_ctx_t* const );
	RT_ERR (*one_param_string) ( jased_ctx_t* const, string_buffer_t* );
} command_t;

typedef struct executor {
	command_t command;
} executor_t;

#endif
