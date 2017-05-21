#include "jased/runtime/executors_constructors.h"

#define EXECUTOR_INIT( args_, ctx_, cmd_, run_, clean_ ) \
	new_executor-> rt_ctx.args_for.any_args = args_; \
	new_executor-> rt_ctx.args_for.jased_ctx = ctx_; \
	new_executor-> command.any_command = cmd_; \
	new_executor-> run = run_; \
	new_executor-> clean = clean_


executor_t* construct_regexsub_executor( 
	jased_ctx_t* const jased_ctx, 
	regex_t const regex, 
	string_buffer_t* const repl, 
	int const flags 
) {

	struct regex_sub_args* args = 
		(struct regex_sub_args*)malloc( sizeof(struct regex_sub_args) );

	args-> regex = regex;
	args-> replacement = repl;
	args-> flags = flags;

	executor_t* new_executor = executor_new();

	EXECUTOR_INIT(
		(void*)args, jased_ctx, NULL,
		exec_regex_sub, clean_regex_sub
	);

	return new_executor;
}

executor_t* construct_no_params_executor( 
	jased_ctx_t* const jased_ctx,
	no_params_cmd_t command
) {
	executor_t* new_executor = executor_new();

	EXECUTOR_INIT(
		NULL, jased_ctx, command,
		exec_no_params, clean_no_params 
	);

	return new_executor;
}

executor_t* construct_one_param_str_executor( 
	jased_ctx_t* const jased_ctx, 
	string_buffer_t* const str,
	string_param_cmd_t command
) {
	struct string_param_args* args = 
		(struct string_param_args*)malloc( sizeof(struct string_param_args) );	

	args-> string = str;

	executor_t* new_executor = executor_new();

	EXECUTOR_INIT(
		args, jased_ctx, command,
		exec_one_param_str, clean_one_param_str
	);

	return new_executor;
}

executor_t* construct_line_condition(
	jased_ctx_t* const jased_ctx,
	int const line_num,
	int const on_fail_cmd
) {
	condition_args_t* args = (condition_args_t*)malloc( sizeof(condition_args_t) );

	args-> line_condition_args 
		= (struct line_condition_args_t*)malloc( sizeof(struct line_condition_args) );

	args-> line_condition_args-> line = line_num;
	args-> line_condition_args-> if_false_cmd_ptr = on_fail_cmd;

	executor_t* new_executor = executor_new();

	EXECUTOR_INIT(
		args, jased_ctx, NULL,
		exec_line_condition, 
		clean_line_condition
	);

	return new_executor;

}

executor_t* construct_line_range_condition(
	jased_ctx_t* const jased_ctx,
	int const line_start,
	int const line_end,
	int const on_fail_cmd
) {
	condition_args_t* args = (condition_args_t*)malloc( sizeof(condition_args_t) );

	args-> line_range_condition_args 
		= (struct line_range_condition_args*)malloc( sizeof(struct line_range_condition_args) );

	args-> start = line_start;
	args-> end = line_end;
	args-> if_false_cmd =  on_fail_cmd;

	executor_t* new_executor = executor_new();

	EXECUTOR_INIT(
		args, jased_ctx, NULL, 
		exec_line_range_condition, 
		clean_line_range_condition
	);

	return new_executor;
}

executor_t* construct_regmatch_condition(
	jased_ctx_t* const jased_ctx,
	regex_t const regex,
	int const on_fail_cmd
) {
	
	condition_args_t* args = (condition_args_t*)malloc( sizeof(condition_args_t) );

	args-> regmatch_condition_args 
		= (struct regmatch_condition_args*)malloc( sizeof(struct regmatch_condition_args) );

	args-> regex = regex;
	args-> if_false_cmd_ptr = on_fail_cmd;

	executor_t* new_executor = executor_new();
	
	EXECUTOR_INIT(
		args, jased_ctx, NULL, 
		exec_regmatch_condition, 
		clean_regmatch_condition	
	);

	return new_executor;
}

executor_t* construct_regmatch_range_condition(
	jased_ctx_t* const jased_ctx,
	regex_t const regstart,
	regex_t const regend,
	int const on_fail_cmd
) {
	condition_args_t* args = (condition_args_t*)malloc( sizeof(condition_args_t) );

	args-> regmatch_range_condition_args 
		= (struct regmatch_range_condition_args*)malloc( sizeof(struct regmatch_range_condition_args* );

	args-> start = regstart;
	args-> end = regend;
	args-> if_false_cmd_ptr = on_fail_cmd;

	executor_t* new_executor = executor_new();
	
	EXECUTOR_INIT(
		args, jased_ctx, NULL, 
		exec_regmatch_range_condition, 
		clean_regmatch_range_condition	
	);

	return new_executor;
}

executor_t* construct_line_regmatch_condition(
	jased_ctx_t* const jased_ctx,
	int const start,
	regex_t const regend,
	int const on_fail_cmd
) {
	condition_args_t* args = (condition_args_t*)malloc( sizeof(condition_args_t) );

	args-> line_regmatch_range_condition_args 
		= (struct line_regmatch_range_condition_args*)
		malloc( sizeof(struct line_regmatch_range_condition_args* );

	args-> start = start;
	args-> end = regend;
	args-> if_false_cmd_ptr = on_fail_cmd;

	executor_t* new_executor = executor_new();
	
	EXECUTOR_INIT(
		args, jased_ctx, NULL, 
		exec_line_regmatch_condition, 
		clean_line_regmatch_condition	
	);

	return new_executor;
}

executor_t* construct_line_regmatch_condition(
	jased_ctx_t* const jased_ctx,
	regex_t const regstart,
	int const end,
	int const on_fail_cmd
) {
	condition_args_t* args = (condition_args_t*)malloc( sizeof(condition_args_t) );

	args-> regmatch_line_range_condition_args 
		= (struct regmatch_line_range_condition_args*)
		malloc( sizeof(struct regmatch_line_range_condition_args* );

	args-> start = regstart;
	args-> end = end;
	args-> if_false_cmd_ptr = on_fail_cmd;

	executor_t* new_executor = executor_new();
	
	EXECUTOR_INIT(
		args, jased_ctx, NULL, 
		exec_regmatch_line_condition, 
		clean_regmatch_line_condition	
	);

	return new_executor;
}
