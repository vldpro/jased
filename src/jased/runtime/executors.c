#include <stddef.h>
#include <malloc.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>


#include "jased/runtime/executors.h"

/* 	
	Executors - abstract objects which contains jased commands with their arguments.
	Runners - functions which iterpret executors.
	Cleaners - functions which clean executors mem.	
*/

#define DEFINE_RUNNER( name ) \
RT_ERR name( executor_t* const executor )

#define DEFINE_CLEANER( name ) \
void name( executor_t* executor )

executor_t* executor_new() {
	return (executor_t*)malloc( sizeof(executor_t) );
}

DEFINE_RUNNER( exec_no_params ) {
	runtime_ctx_t const rt_ctx = executor-> rt_ctx;

	return executor-> command.run_no_args( 
		rt_ctx.jased_ctx 
	);
}

DEFINE_CLEANER( clean_no_params ) {
	free( executor );	
}


DEFINE_RUNNER( exec_transform ) {
	runtime_ctx_t const rt_ctx = executor-> rt_ctx;

	return executor-> command.run_transform(
		rt_ctx.jased_ctx,
		rt_ctx.args_for.tran_args-> to_transform,
		rt_ctx.args_for.tran_args-> transform_seq
	);
}

DEFINE_CLEANER( clean_transform ) {
	runtime_ctx_t const rt_ctx = executor-> rt_ctx;

	sbuffer_delete( rt_ctx.args_for.tran_args-> to_transform );
	sbuffer_delete( rt_ctx.args_for.tran_args-> transform_seq );
	free( rt_ctx.args_for.tran_args );
	free( executor );
}



DEFINE_RUNNER( exec_one_param_str ) {
	runtime_ctx_t const rt_ctx = executor-> rt_ctx;

	return executor-> command.run_str_arg( 
		rt_ctx.jased_ctx, 
		rt_ctx.args_for.string_param-> string 
	);
}

DEFINE_CLEANER( clean_one_param_str ) {
	runtime_ctx_t const rt_ctx = executor-> rt_ctx;

	sbuffer_delete( rt_ctx.args_for.string_param-> string );
	free( rt_ctx.args_for.string_param );
	free( executor );
}


DEFINE_RUNNER( exec_one_param_int ) {
	runtime_ctx_t const rt_ctx = executor-> rt_ctx;

	return executor-> command.run_int_arg( 
		rt_ctx.jased_ctx, 
		rt_ctx.args_for.int_param-> int_value 
	);
}

DEFINE_CLEANER( clean_one_param_int ) {
	runtime_ctx_t const rt_ctx = executor-> rt_ctx;

    close_file( rt_ctx.args_for.int_param-> int_value );
	free( rt_ctx.args_for.int_param );
	free( executor );
}


DEFINE_RUNNER( exec_regex_sub ) {
	struct regex_sub_args* const regsub_args = executor-> rt_ctx.args_for.reg_sub;

	return executor-> command.run_regsub(
		executor-> rt_ctx.jased_ctx,
		regsub_args-> regex,
		regsub_args-> replacement,
		regsub_args-> flags,
        regsub_args-> match_num,
        regsub_args-> wfile
	);
}


DEFINE_CLEANER( clean_regex_sub ) {
	struct regex_sub_args* const regsub_args = executor-> rt_ctx.args_for.reg_sub;

	regfree( &(regsub_args-> regex) );
    if ( regsub_args-> wfile != -1 ) close( regsub_args-> wfile ); 
	sbuffer_delete( regsub_args-> replacement );	
	free( regsub_args );
	free( executor );
}

/* CONDITIONS executors */

condition_args_t* condition_args_new() {
	return (condition_args_t*)malloc( sizeof(condition_args_t) );
}

#define XOR( a, b ) \
    (!(a) && (b)) || (!(b) && (a))

DEFINE_RUNNER( exec_line_condition ) {
	struct line_condition_args* const args = executor-> rt_ctx.args_for.condition_args-> line_cond_args;
	jased_ctx_t* jased_ctx = executor-> rt_ctx.jased_ctx;

    int condition = args-> line != ULONG_MAX ? 
        jased_ctx-> current_line != args-> line : !jased_ctx-> is_last_line;

	if ( XOR(args-> is_negative, condition) ) {
		jased_ctx-> command_pointer = args-> if_false_cmd_ptr;
	}

	return 0;
}

DEFINE_CLEANER( clean_line_condition ) {
	free( executor-> rt_ctx.args_for.condition_args-> line_cond_args );
	free( executor-> rt_ctx.args_for.condition_args );
	free( executor );
}


DEFINE_RUNNER( exec_line_range_condition ) {
	struct lines_range_condition_args* const args = 
		executor-> rt_ctx.args_for.condition_args-> lines_range_cond_args;

	jased_ctx_t* jased_ctx = executor-> rt_ctx.jased_ctx;

    int start_condition = args-> start != ULONG_MAX ? 
        jased_ctx-> current_line < args-> start : !jased_ctx-> is_last_line;

    int end_condition = jased_ctx-> current_line > args-> end;

	if ( XOR(args-> is_negative, start_condition || end_condition) ) {
		jased_ctx-> command_pointer = args-> if_false_cmd_ptr;
	}

	return 0;
}

DEFINE_CLEANER( clean_line_range_condition ) {
	free( executor-> rt_ctx.args_for.condition_args-> lines_range_cond_args );
	free( executor-> rt_ctx.args_for.condition_args );
	free( executor );
}

DEFINE_RUNNER( exec_regmatch_condition ) {
	struct regmatch_condition_args* const args = 
		executor-> rt_ctx.args_for.condition_args-> regmatch_cond_args;

	jased_ctx_t* jased_ctx = executor-> rt_ctx.jased_ctx;

	if ( XOR(args-> is_negative, !match(jased_ctx-> pattern_space-> char_at, args-> regex)) ) {
		jased_ctx-> command_pointer = args-> if_false_cmd_ptr;
	}

	return 0;
}

DEFINE_CLEANER( clean_regmatch_condition ) {
	regfree( &(executor-> rt_ctx.args_for.condition_args-> regmatch_cond_args-> regex) );
	free( executor-> rt_ctx.args_for.condition_args-> regmatch_cond_args );
	free( executor-> rt_ctx.args_for.condition_args );
	free( executor );
}

DEFINE_RUNNER( exec_regmatch_range_condition ) {
	struct regmatch_range_condition_args* const args = 
		executor-> rt_ctx.args_for.condition_args-> regmatch_range_cond_args;

	jased_ctx_t* jased_ctx = executor-> rt_ctx.jased_ctx;

	 
    if ( !args-> is_start_matched ) {
		if ( match(jased_ctx-> pattern_space-> char_at, args-> start) ) {
			args-> is_start_matched = 1;
            if ( args-> is_negative ) jased_ctx-> command_pointer = args-> if_false_cmd_ptr;
		} else if ( !args-> is_negative ) {
            jased_ctx-> command_pointer = args-> if_false_cmd_ptr;
        }

	} else if ( !args-> is_end_matched ) {
		if ( match(jased_ctx-> pattern_space-> char_at, args-> end) ) {
			args-> is_end_matched = 1;
		} 

        if ( args-> is_negative ) jased_ctx-> command_pointer = args-> if_false_cmd_ptr;

	} else if ( XOR(args-> is_negative, !args-> is_start_matched || args-> is_end_matched) ) {
		jased_ctx-> command_pointer = args-> if_false_cmd_ptr;
	}

	return 0;	
}

DEFINE_CLEANER( clean_regmatch_range_condition ) {
		regfree( &(executor-> rt_ctx.args_for.condition_args-> regmatch_range_cond_args-> start) );
		regfree( &(executor-> rt_ctx.args_for.condition_args-> regmatch_range_cond_args-> end) );
		free( executor-> rt_ctx.args_for.condition_args-> regmatch_range_cond_args );
		free( executor-> rt_ctx.args_for.condition_args );
		free( executor );
}

DEFINE_RUNNER( exec_line_regmatch_condition ) {
	struct line_regmatch_range_condition_args* const args = 
		executor-> rt_ctx.args_for.condition_args-> line_regmatch_range_cond_args;

	jased_ctx_t* jased_ctx = executor-> rt_ctx.jased_ctx;
	
    if ( !args-> is_end_matched ) {
		if ( match(jased_ctx-> pattern_space-> char_at, args-> end) ) {
			args-> is_end_matched = 1;
		} 

        if ( args-> is_negative ) jased_ctx-> command_pointer = args-> if_false_cmd_ptr;
        
	} else if ( XOR(args-> is_negative, jased_ctx-> current_line >= args-> start && args-> is_end_matched) ) {
		jased_ctx-> command_pointer = args-> if_false_cmd_ptr;
	}

	return 0;
}

DEFINE_CLEANER( clean_line_regmatch_condition ) {
	regfree( &(executor-> rt_ctx.args_for.condition_args-> line_regmatch_range_cond_args-> end) );
	free( executor-> rt_ctx.args_for.condition_args-> line_regmatch_range_cond_args );
	free( executor-> rt_ctx.args_for.condition_args );
	free( executor );
}

DEFINE_RUNNER( exec_regmatch_line_condition ) {
	struct regmatch_line_range_condition_args* const args = 
		executor-> rt_ctx.args_for.condition_args-> regmatch_line_range_cond_args;

	jased_ctx_t* jased_ctx = executor-> rt_ctx.jased_ctx;
	
    if ( !args-> is_start_matched )  {
		if ( match(jased_ctx-> pattern_space-> char_at, args-> start) ) {
			args-> is_start_matched = 1;
            if ( args-> is_negative ) jased_ctx-> command_pointer = args-> if_false_cmd_ptr;
		} else {
		    if ( !args-> is_negative ) jased_ctx-> command_pointer = args-> if_false_cmd_ptr;
        }
	} else if ( XOR(args-> is_negative, args-> is_start_matched && args-> end < jased_ctx-> current_line) ) {
		jased_ctx-> command_pointer = args-> if_false_cmd_ptr;

	}

	return 0;
}

DEFINE_CLEANER( clean_regmatch_line_condition ) {
	regfree( &(executor-> rt_ctx.args_for.condition_args-> regmatch_line_range_cond_args-> start) );
	free( executor-> rt_ctx.args_for.condition_args-> regmatch_line_range_cond_args );
	free( executor-> rt_ctx.args_for.condition_args );
	free( executor );
}
