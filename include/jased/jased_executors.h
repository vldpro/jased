#ifndef JASED_EXECUTORS
#define JASED_EXECUTORS

#include "jased_regex.h"
#include "jased_string_buffer.h"
#include "jased_context.h"
#include "jased_conditions.h"

#define RT_ERR int 

struct regex_match_args {
	regex_t regex;
}; 

struct regex_sub_args {
	regex_t regex;
	string_buffer_t* replacement;
	int flags;
};

struct string_param_args {
	string_buffer_t* string;
};

typedef union cmd_args {
	struct regex_match_args*     reg_match;
	struct regex_sub_args*   	 reg_sub;
	struct string_param_args*    string_param;
	condition_args_t*     condition; 
} cmd_args_t;

typedef struct runtime_ctx {
	jased_ctx_t* jased_ctx;
	cmd_args_t args_for;		
} runtime_ctx_t;


typedef RT_ERR (*no_params_cmd_t) ( jased_ctx_t* const );
typedef RT_ERR (*string_param_cmd_t) ( jased_ctx_t* const, string_buffer_t* const );
typedef RT_ERR (*regex_match_cmd_t) ( jased_ctx_t* const, regex_t const );
typedef RT_ERR (*regex_sub_cmd_t) ( jased_ctx_t* const, regex_t const, string_buffer_t* const, int const );
typedef RT_ERR (*condition_cmd_t) ( jased_ctx_t* const, condition_args_t* cond_args  );

typedef union cmd_type {
	no_params_cmd_t    run_no_args;	
	string_param_cmd_t run_str_arg;
	regex_match_cmd_t  run_regmatch;
	regex_sub_cmd_t    run_regsub;
	condition_cmd_t    run_condition;
} cmd_type_t;

typedef struct executor {
	runtime_ctx_t rt_ctx;
	cmd_type_t command;
	RT_ERR (*exec_command) ( struct executor* const );
} executor_t;

#endif
