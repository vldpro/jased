#include <stddef.h>

#include "jased_executors.h"

#define DEFINE_EXECUTOR( name ) \
RT_ERR name( executor_t* const executor )

executor_t* executor_new() {
	return (executor_t*)malloc( sizeof(executor_t) );
}

void executor_delete( executor_t* const executor ) {
	free( executor );
}

DEFINE_EXECUTOR( exec_no_params ) {
	runtime_ctx_t const rt_ctx = executor-> rt_ctx;

	return executor-> command.run_no_args( 
		rt_ctx.jased_ctx 
	);
}

DEFINE_EXECUTOR( exec_one_param_str ) {
	runtime_ctx_t const rt_ctx = executor-> rt_ctx;

	return executor-> command.run_str_arg( 
		rt_ctx.jased_ctx, 
		rt_ctx.args_for.string_param-> string 
	);
}

DEFINE_EXECUTOR( exec_regex_match ) {
	struct regex_match_args* const regmatch_args = executor-> rt_ctx.args_for.reg_match;

	return executor-> command.run_regmatch( 
		executor-> rt_ctx.jased_ctx, 
		regmatch_args-> regex 
	);
}

DEFINE_EXECUTOR( exec_regex_sub ) {
	struct regex_sub_args* const regsub_args = executor-> rt_ctx.args_for.reg_sub;

	return executor-> command.run_regsub(
		executor-> rt_ctx.jased_ctx,
		regsub_args-> regex,
		regsub_args-> replacement,
		regsub_args-> flags
	);
}

DEFINE_EXECUTOR( exec_condition ) {
	runtime_ctx_t const rt_ctx = executor-> rt_ctx;

	return executor-> command.run_condition(
		rt_ctx.jased_ctx,
		rt_ctx.args_for.condition
	);

}
