#ifndef JASED_EXECUTORS
#define JASED_EXECUTORS

#include <stddef.h>

#include "jased/commands/regex.h"
#include "jased/util/string_buffer.h"

#include "jased/runtime/context.h"
#include "jased/runtime/conditions.h"

#define RT_ERR int 

/*                 
     Executors - abstract objects which contains jased commands with their arguments.
     Runners - functions which iterpret executors.      
     Cleaners - functions which clean executors mem. 
*/ 

struct regex_sub_args {
	regex_t regex;
	string_buffer_t* replacement;

	int flags;
    int match_num;
    int wfile;
};

struct transform_args {
	string_buffer_t* to_transform;
	string_buffer_t* transform_seq;
};

struct string_param_args {
	string_buffer_t* string;
};

struct int_param_args {
    int int_value;
};

typedef union cmd_args {
	void* 						 any_args;
	struct transform_args*		 tran_args;
	struct regex_sub_args*   	 reg_sub;
	struct string_param_args*    string_param;
    struct int_param_args*       int_param;
	condition_args_t*     		 condition_args; 
} cmd_args_t;

typedef struct runtime_ctx {
	jased_ctx_t* jased_ctx;
	cmd_args_t args_for;		
} runtime_ctx_t;


typedef RT_ERR (*no_params_cmd_t) ( jased_ctx_t* const );
typedef RT_ERR (*string_param_cmd_t) ( jased_ctx_t* const, string_buffer_t* const );
typedef RT_ERR (*int_param_cmd_t) ( jased_ctx_t* const, int const );
typedef RT_ERR (*regex_sub_cmd_t) ( jased_ctx_t* const, regex_t const, string_buffer_t* const, int const, int const, int const );
typedef RT_ERR (*transform_cmd_t) ( jased_ctx_t* const, string_buffer_t* const, string_buffer_t* const );

typedef union cmd_type {
	void* 			   any_command;
	no_params_cmd_t    run_no_args;	
	string_param_cmd_t run_str_arg;
	regex_sub_cmd_t    run_regsub;
	transform_cmd_t    run_transform;
    int_param_cmd_t    run_int_arg;
} cmd_type_t;

typedef struct executor {
	runtime_ctx_t rt_ctx;
	cmd_type_t command;
	RT_ERR (*run) ( struct executor* const );
	void (*clean) ( struct executor* );
} executor_t;

#define DECLARE_RUNNER( name ) \
RT_ERR name( executor_t* const executor )

#define DECLARE_CLEANER( name ) \
void name( executor_t* executor )

/* CONSTRUCTORS */

executor_t* executor_new();

executor_t* construct_regexsub_executor( 
	jased_ctx_t* const jased_ctx, 
	regex_sub_cmd_t command,
	regex_t const regex, 
	string_buffer_t* const repl, 
	int const flags,
    int const match_num,
    string_buffer_t* wfile
);

executor_t* construct_io_executor(
    jased_ctx_t* const jased_ctx,
    int_param_cmd_t command,
	string_buffer_t* const filename
);

executor_t* construct_transform_executor(
	jased_ctx_t* const jased_ctx,
	transform_cmd_t command,
	string_buffer_t* const to_transform,
	string_buffer_t* const transform_seq
);

executor_t* construct_no_params_executor( 
	jased_ctx_t* const jased_ctx,
	no_params_cmd_t command
);

executor_t* construct_one_param_str_executor( 
	jased_ctx_t* const jased_ctx, 
	string_param_cmd_t command,
	string_buffer_t* const str
);

executor_t* construct_line_condition(
	jased_ctx_t* const jased_ctx,
	int const line_num,
	int const on_fail_cmd,
    int const is_negative
);

executor_t* construct_line_range_condition(
	jased_ctx_t* const jased_ctx,
	int const line_start,
	int const line_end,
	int const on_fail_cmd,
    int const is_negative
);

executor_t* construct_regmatch_condition(
	jased_ctx_t* const jased_ctx,
	regex_t const regex,
	int const on_fail_cmd,
    int const is_negative
);

executor_t* construct_regmatch_range_condition(
	jased_ctx_t* const jased_ctx,
	regex_t const regstart,
	regex_t const regend,
	int const on_fail_cmd,
    int const is_negative
);

executor_t* construct_line_regmatch_condition(
	jased_ctx_t* const jased_ctx,
	int const start,
	regex_t const regend,
	int const on_fail_cmd,
    int const is_negative
);

executor_t* construct_regmatch_line_condition(
	jased_ctx_t* const jased_ctx,
	regex_t const regstart,
	int const end,
	int const on_fail_cmd,
    int const is_negative
);

/* RUNNERS AND CLEANERS */


DECLARE_RUNNER( exec_no_params );
DECLARE_CLEANER( clean_no_params );

DECLARE_RUNNER( exec_one_param_str );
DECLARE_CLEANER( clean_one_param_str );


DECLARE_RUNNER( exec_one_param_int );
DECLARE_CLEANER( clean_one_param_int );

DECLARE_RUNNER( exec_regex_sub );
DECLARE_CLEANER( clean_regex_sub );

DECLARE_RUNNER( exec_transform);
DECLARE_CLEANER( clean_transform );

/* CONDITIONS */

DECLARE_RUNNER( exec_line_condition );
DECLARE_CLEANER( clean_line_condition );

DECLARE_RUNNER( exec_line_range_condition );
DECLARE_CLEANER( clean_line_range_condition );

DECLARE_RUNNER( exec_regmatch_condition );
DECLARE_CLEANER( clean_regmatch_condition );

DECLARE_RUNNER( exec_regmatch_range_condition );
DECLARE_CLEANER( clean_regmatch_range_condition );

DECLARE_RUNNER( exec_line_regmatch_condition );
DECLARE_CLEANER( clean_line_regmatch_condition );

DECLARE_RUNNER( exec_regmatch_line_condition );
DECLARE_CLEANER( clean_regmatch_line_condition );

#endif
