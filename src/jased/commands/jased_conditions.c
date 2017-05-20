#include "jased_conditions.h"
#include "jased_executors.h"
#include "jased_regex.h"

#define DEFINE_CONDITION_EXECUTOR( name ) \
RT_ERR name( jased_ctx_t* const jased_ctx, condition_args_t* const condition_args )

DEFINE_CONDITION_EXECUTOR( exec_line_condition ) {
	struct line_condition_args* const args = condition_args-> line_cond_args;

	if ( jased_ctx-> current_line != args-> line ) {
		jased_ctx-> command_pointer = args-> if_false_cmd_ptr;
	}

	return 0;
}

DEFINE_CONDITION_EXECUTOR( exec_line_range_condition ) {
	struct lines_range_condition_args* const args = condition_args-> lines_range_cond_args;

	if ( jased_ctx-> current_line <= args-> start 
		&& jased_ctx-> current_line >= args-> end ) {
		
		jased_ctx-> command_pointer = args-> if_false_cmd_ptr;
	}

	return 0;
}

DEFINE_CONDITION_EXECUTOR( exec_regmatch_condition ) {
	struct regmatch_condition_args* const args = condition_args-> regmatch_cond_args;

	if ( !match( jased_ctx-> pattern_buffer-> char_at, args-> regex ) ) {
		jased_ctx-> command_pointer = args-> if_false_cmd_ptr;
	}

	return 0;
}

DEFINE_CONDITION_EXECUTOR( exec_regmatch_range_condition ) {
	struct regmatch_range_condition_args* const args = condition_args-> regmatch_range_cond_args;

	if ( args-> is_start_matched && args-> is_end_matched ) {
		jased_ctx-> command_pointer = args-> if_false_cmd_ptr;
		
	} else if ( !args-> is_start_matched ) {
		if ( !match(jased_ctx-> pattern_buffer-> char_at, args-> start) ) {
			jased_ctx-> command_pointer = args-> if_false_cmd_ptr;
		} else {
			args-> is_start_matched = MATCH;
		}

	} else if ( !args-> is_end_matched ) {
		if ( !match(jased_ctx-> pattern_buffer-> char_at, args-> end) ) {
			jased_ctx-> command_pointer = args-> if_false_cmd_ptr;
		} else {
			args-> is_end_matched = MATCH;
		} 
	}

	return 0;	
}

DEFINE_CONDITION_EXECUTOR( exec_line_regmatch_cond_args ) {
	struct line_regmatch_range_condition_args* const args = condition_args-> line_regmatch_range_cond_args;

	if ( jased_ctx-> current_line < args-> start || args-> is_end_matched ) {
		jased_ctx-> command_pointer = args-> if_false_cmd_ptr;

	} else if ( !args-> is_end_matched ) {
		if ( !match(jased_ctx-> pattern_buffer-> char_at, args-> end) ) {
			jased_ctx-> command_pointer = args-> if_false_cmd_ptr;
		} else {
			args-> is_end_matched = MATCH;
		} 	
	}

	return 0;
}

DEFINE_CONDITION_EXECUTOR( exec_regmatch_line_cond_args ) {
	struct regmatch_line_range_condition_args* const args = condition_args-> regmatch_line_range_cond_args;

	if ( args-> is_start_matched && args-> end < jased_ctx-> current_line ) {
		jased_ctx-> command_pointer = args-> if_false_cmd_ptr;

	} else if ( !args-> is_start_matched )  {
		if ( !match(jased_ctx-> pattern_buffer-> char_at, args-> start) ) {
			jased_ctx-> command_pointer = args-> if_false_cmd_ptr;
		} else {
			args-> is_start_matched = MATCH;
		}
	}

	return 0;
}
