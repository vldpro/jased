#ifndef JASED_EXECUTORS
#define JASED_EXECUTORS

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
};

struct string_param_args {
	string_buffer_t* string;
};

typedef union cmd_args {
	struct regex_sub_args*   	 reg_sub;
	struct string_param_args*    string_param;
	condition_args_t*     		 condition; 
} cmd_args_t;

typedef struct runtime_ctx {
	jased_ctx_t* jased_ctx;
	cmd_args_t args_for;		
} runtime_ctx_t;


typedef RT_ERR (*no_params_cmd_t) ( jased_ctx_t* const );
typedef RT_ERR (*string_param_cmd_t) ( jased_ctx_t* const, string_buffer_t* const );
typedef RT_ERR (*regex_sub_cmd_t) ( jased_ctx_t* const, regex_t const, string_buffer_t* const, int const );

typedef union cmd_type {
	no_params_cmd_t    run_no_args;	
	string_param_cmd_t run_str_arg;
	regex_sub_cmd_t    run_regsub;
} cmd_type_t;

typedef struct executor {
	runtime_ctx_t rt_ctx;
	cmd_type_t command;
	RT_ERR (*run) ( struct executor* const );
	void (*clean) ( struct executor* );
} executor_t;


#define DECLARE_RUNNER( name ) \
RT_ERR name( executor_t* const executor );

#define DECLARE_CLEANER( name ) \
void name( executor_t* executor );

executor_t* executor_new();

DECLARE_RUNNER( exec_no_params );
DECLARE_CLEANER( clean_no_params );

DECLARE_RUNNER( exec_one_param_str );
DECLARE_CLEANER( clean_one_param_str );

DECLARE_RUNNER( exec_regex_sub );
DECLARE_CLEANER( clean_regex_sub );


DECLARE_RUNNER( exec_regex_sub );
DECLARE_CLEANER( clean_regex_sub );

/* CONDITIONS */

DECLARE_RUNNER( exec_line_condition );
DECLARE_CLEANER( clean_line_condition );

DECLARE_RUNNER( exec_line_range_condition );
DECLARE_CLEANER( clean_line_range_condition );

DECLARE_RUNNER( exec_regmatch_condition );
DECLARE_CLEANER( clean_regmatch_condition );

DECLARE_RUNNER( exec_regmatch_range_condtion );
DECLARE_CLEANER( clean_regmatch_range_condtion );

DECLARE_RUNNER( exec_line_regmatch_condition );
DECLARE_CLEANER( clean_line_regmatch_condition );

DECLARE_RUNNER( exec_regmatch_line_condition );
DECLARE_CLEANER( clean_regmatch_line_condition );

#endif
