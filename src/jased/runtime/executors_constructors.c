#include "jased/runtime/executors.h"
#include "jased/io/io.h"

#include <unistd.h> 
#include <errno.h> 
#include <string.h> 

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#define EXECUTOR_INIT( args_, ctx_, run_, clean_ ) \
	new_executor-> rt_ctx.args_for.any_args = args_; \
	new_executor-> rt_ctx.jased_ctx = ctx_; \
	new_executor-> run = run_; \
	new_executor-> clean = clean_


executor_t* construct_regexsub_executor( 
	jased_ctx_t* const jased_ctx, 
	regex_sub_cmd_t command,
	regex_t const regex, 
	string_buffer_t* const repl, 
	int const flags,
    int const match_num,
    string_buffer_t* wfile
) {

	struct regex_sub_args* args = 
		(struct regex_sub_args*)malloc( sizeof(struct regex_sub_args) );
	executor_t* new_executor = executor_new();

	args-> regex = regex;
	args-> replacement = repl;
	args-> flags = flags;
    args-> match_num = match_num;
    if ( wfile != NULL ) {
        args-> wfile = open( wfile-> char_at, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR );

        if ( args-> wfile == -1 ) { 
            int errnum = errno;
            printerr("jased: can not open file '");
            printerr( wfile-> char_at );
            printerr("' : ");
            printerr( strerror(errnum) );
            printerr("\n");
            exit(4);
        }

        sbuffer_delete( wfile );
    } else args-> wfile = -1;


	new_executor-> command.run_regsub = command;

	EXECUTOR_INIT(
		(void*)args, jased_ctx,
		exec_regex_sub, clean_regex_sub
	);

	return new_executor;
}

executor_t* construct_transform_executor(
	jased_ctx_t* const jased_ctx,
	transform_cmd_t command,
	string_buffer_t* const to_transform,
	string_buffer_t* const transform_seq
) {
	struct transform_args* args = 
		(struct transform_args*)malloc( sizeof(struct transform_args) );
	executor_t* new_executor = executor_new();

	args-> to_transform = to_transform;
	args-> transform_seq = transform_seq;

	new_executor-> command.run_transform = command;

	EXECUTOR_INIT(
		(void*)args, jased_ctx,
		exec_transform, clean_transform
	);

	return new_executor;

}

executor_t* construct_no_params_executor( 
	jased_ctx_t* const jased_ctx,
	no_params_cmd_t command
) {
	executor_t* new_executor = executor_new();

	new_executor-> command.run_no_args = command;

	EXECUTOR_INIT(
		NULL, jased_ctx,
		exec_no_params, 
		clean_no_params 
	);

	return new_executor;
}

executor_t* construct_one_param_str_executor( 
	jased_ctx_t* const jased_ctx, 
	string_param_cmd_t command,
	string_buffer_t* const str
) {
	struct string_param_args* args = 
		(struct string_param_args*)malloc( sizeof(struct string_param_args) );	
	executor_t* new_executor = executor_new();

	args-> string = str;

	new_executor-> command.run_str_arg = command;

	EXECUTOR_INIT(
		args, jased_ctx,
		exec_one_param_str, 
		clean_one_param_str
	);

	return new_executor;
}

executor_t* construct_io_executor( 
	jased_ctx_t* const jased_ctx, 
	int_param_cmd_t command,
	string_buffer_t* const filename
) {
	struct int_param_args* args = 
		(struct int_param_args*)malloc( sizeof(struct int_param_args) );	
	executor_t* new_executor = executor_new();

	args-> int_value = open_filem( filename-> char_at, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR );
    sbuffer_delete(filename);

	new_executor-> command.run_int_arg = command;

	EXECUTOR_INIT(
		args, jased_ctx,
		exec_one_param_int, 
		clean_one_param_int
	);

	return new_executor;
}

executor_t* construct_line_condition(
	jased_ctx_t* const jased_ctx,
	int const line_num,
	int const on_fail_cmd
) {
	condition_args_t* args = (condition_args_t*)malloc( sizeof(condition_args_t) );
	executor_t* new_executor = executor_new();

	args-> line_cond_args 
		= (struct line_condition_args*)malloc( sizeof(struct line_condition_args) );

	args-> line_cond_args-> line = line_num;
	args-> line_cond_args-> if_false_cmd_ptr = on_fail_cmd;


	EXECUTOR_INIT(
		args, jased_ctx,
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
	executor_t* new_executor = executor_new();

	args-> lines_range_cond_args 
		= (struct lines_range_condition_args*)malloc( sizeof(struct lines_range_condition_args) );

	args-> lines_range_cond_args-> start = line_start;
	args-> lines_range_cond_args-> end = line_end;
	args-> lines_range_cond_args-> if_false_cmd_ptr =  on_fail_cmd;


	EXECUTOR_INIT(
		args, jased_ctx,
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
	executor_t* new_executor = executor_new();

	args-> regmatch_cond_args 
		= (struct regmatch_condition_args*)malloc( sizeof(struct regmatch_condition_args) );

	args-> regmatch_cond_args-> regex = regex;
	args-> regmatch_cond_args-> if_false_cmd_ptr = on_fail_cmd;
	
	EXECUTOR_INIT(
		args, jased_ctx,
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
	executor_t* new_executor = executor_new();

	args-> regmatch_range_cond_args 
		= (struct regmatch_range_condition_args*)malloc( sizeof(struct regmatch_range_condition_args) );

	args-> regmatch_range_cond_args-> start = regstart;
	args-> regmatch_range_cond_args-> end = regend;
	args-> regmatch_range_cond_args-> if_false_cmd_ptr = on_fail_cmd;
	args-> regmatch_range_cond_args-> is_start_matched = 0;
	args-> regmatch_range_cond_args-> is_end_matched = 0;

	
	EXECUTOR_INIT(
		args, jased_ctx,
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
	executor_t* new_executor = executor_new();

	args-> line_regmatch_range_cond_args 
		= (struct line_regmatch_range_condition_args*)
		malloc( sizeof(struct line_regmatch_range_condition_args) );

	args-> line_regmatch_range_cond_args-> start = start;
	args-> line_regmatch_range_cond_args-> end = regend;
	args-> line_regmatch_range_cond_args-> if_false_cmd_ptr = on_fail_cmd;
	args-> line_regmatch_range_cond_args-> is_end_matched = 0;
	
	EXECUTOR_INIT(
		args, jased_ctx,
		exec_line_regmatch_condition, 
		clean_line_regmatch_condition	
	);

	return new_executor;
}

executor_t* construct_regmatch_line_condition(
	jased_ctx_t* const jased_ctx,
	regex_t const regstart,
	int const end,
	int const on_fail_cmd
) {
	condition_args_t* args = (condition_args_t*)malloc( sizeof(condition_args_t) );
	executor_t* new_executor = executor_new();

	args-> regmatch_line_range_cond_args 
		= (struct regmatch_line_range_condition_args*)
		malloc( sizeof(struct regmatch_line_range_condition_args) );

	args-> regmatch_line_range_cond_args-> start = regstart;
	args-> regmatch_line_range_cond_args-> end = end;
	args-> regmatch_line_range_cond_args-> if_false_cmd_ptr = on_fail_cmd;
	args-> regmatch_line_range_cond_args-> is_start_matched = 0;
	
	EXECUTOR_INIT(
		args, jased_ctx,
		exec_regmatch_line_condition, 
		clean_regmatch_line_condition	
	);

	return new_executor;
}
