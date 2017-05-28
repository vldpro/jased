#include <unistd.h>

#include "jased/parser/line_parser.h"

#include "jased/parser/errors.h"
#include "jased/parser/parser_context.h"
#include "jased/parser/condition_parser.h"
#include "jased/parser/util.h"
#include "jased/parser/terminals.h"

#include "jased/commands/commands.h"
#include "jased/io/io.h"

#include "jased/runtime/executors.h"


#define DEFINE_PARSE_NOARGS_COMMAND( cmd ) \
static parser_status_t cmd( chars_queue_t* const cqueue, interpreter_ctx_t* const int_ctx )

static parser_status_t 
parse_translate( chars_queue_t* cqueue, interpreter_ctx_t* const int_ctx );

static parser_status_t 
parse_sub( chars_queue_t* const cqueue, interpreter_ctx_t* const int_ctx );

static parser_status_t 
parse_append( chars_queue_t* const cqueue, interpreter_ctx_t* const int_ctx );

static parser_status_t
parse_change( chars_queue_t* const cqueue, interpreter_ctx_t* const int_ctx );

static parser_status_t 
parse_insert( chars_queue_t* const cqueue, interpreter_ctx_t* const int_ctx );

static parser_status_t
parse_branch( chars_queue_t* const cqueue, interpreter_ctx_t* const int_ctx );

static parser_status_t
parse_read( chars_queue_t* const cqueue, interpreter_ctx_t* const int_ctx );


static parser_status_t
parse_write( chars_queue_t* const cqueue, interpreter_ctx_t* const int_ctx );

static parser_status_t
parse_label( chars_queue_t* const cqueue, interpreter_ctx_t* const int_ctx );

static void push_noargs_command( no_params_cmd_t cmd, interpreter_ctx_t* const int_ctx );

/* d */
DEFINE_PARSE_NOARGS_COMMAND(parse_delete_ps);

/* D */
DEFINE_PARSE_NOARGS_COMMAND(parse_delete_init_ps);

/* g */
DEFINE_PARSE_NOARGS_COMMAND(parse_replace_ps_by_hs);

/* G */
DEFINE_PARSE_NOARGS_COMMAND(parse_append_hs_to_ps);

/* h */
DEFINE_PARSE_NOARGS_COMMAND(parse_replace_hs_by_ps);

/* H */
DEFINE_PARSE_NOARGS_COMMAND(parse_append_ps_to_hs);

/* n */
DEFINE_PARSE_NOARGS_COMMAND(parse_n);

/* N */
DEFINE_PARSE_NOARGS_COMMAND(parse_append_line_to_ps);

/* p */
DEFINE_PARSE_NOARGS_COMMAND(parse_print);

/* P */
DEFINE_PARSE_NOARGS_COMMAND(parse_println);

/* q */
DEFINE_PARSE_NOARGS_COMMAND(parse_quit);

/* x */
DEFINE_PARSE_NOARGS_COMMAND(parse_exchange);

/* = */
DEFINE_PARSE_NOARGS_COMMAND(parse_print_line_num);


#define TRY_GETCHAR( dest, err_code ) \
	do {  \
		if ( !cqueue_is_empty(cqueue) ) dest = cqueue_getc(cqueue); \
		else return err_code; \
	} while(0)

static executor_t* construct_condition_executor( 
        jased_ctx_t* const jased_ctx, 
        struct condition const* const condition, 
        int const if_false_cmd_ptr 
); 

static parser_status_t parse_command( 
        chars_queue_t* const cqueue, 
        interpreter_ctx_t* const int_ctx,
        parser_ctx_t* const parser_ctx, 
        optional_t condition
); 


parser_status_t parse_line( 
        chars_queue_t* const cqueue, 
        interpreter_ctx_t* const int_ctx,
        parser_ctx_t* const parser_ctx
) {
    optional_t condition;
    condition.cond = malloc( sizeof(struct condition) );
    condition.cond-> linestart = 0;
    condition.cond-> lineend = 0;
    condition.cond-> step = 0;
    condition.is_present = 0;

    while( !cqueue_is_empty(cqueue) ) {
        char sym;
		skip_spaces(cqueue);
		sym = cqueue_gettop(cqueue);

        switch( sym ) {
            case CUSTOM_SEARCH_BEGIN: case DEFAULT_SEARCH_BEGIN: {
                parser_status_t stat;
		        if ( (stat = parse_condition(cqueue, condition.cond)) != PARSING_OK ) {
                    free( condition.cond );
			        return stat;
                }

			    condition.is_present = 1;
                /* break not writed on purpose */
            }

            default: {
                parser_status_t stat;
                /* parse num addr */
                if ( sym >= '0' && sym <= '9' ) {
				    if ( (stat = parse_condition(cqueue, condition.cond)) != PARSING_OK ) {
                        return stat;
                    }

				    condition.is_present = 1;
			    }

                /* parse command */
                if ( (stat = parse_command(cqueue, int_ctx, parser_ctx, condition)) != PARSING_OK ) {
                    free( condition.cond );
			        return stat;
                }

                condition.is_present = 0;
                break;
            }

            case CMD_DELIM: {
			    cqueue_getc(cqueue);
                continue;
            }

            case CMD_DELIM_NEWLINE: {
		    	cqueue_getc(cqueue);
		    	continue; 
		    }

            case COMMENT: {
			    return PARSING_OK;
		    }
        }
    }

    free( condition.cond );

    return PARSING_OK;
}

static executor_t* 
construct_condition_executor( 
	jased_ctx_t* const jased_ctx, 
	struct condition const* const condition, 
	int const if_false_cmd_ptr 
) {
	enum condition_type condition_type = condition-> type;	

	switch( condition_type ) {
		case LINE:
			return construct_line_condition( 
				jased_ctx,
				condition-> linestart,
				if_false_cmd_ptr	
			);

		case RANGE_LINE:
			return construct_line_range_condition(
				jased_ctx,
				condition-> linestart,
				condition-> lineend,
				if_false_cmd_ptr	
			);

		case REGEX:
			return construct_regmatch_condition(
				jased_ctx,
				condition-> regstart,
				if_false_cmd_ptr	
			);

		case RANGE_REGEX:
			return construct_regmatch_range_condition(
				jased_ctx,
				condition-> regstart,
				condition-> regend,
			    if_false_cmd_ptr	
			);

		case RANGE_LINE_REGEX:
			return construct_line_regmatch_condition(
				jased_ctx,
				condition-> linestart,
				condition-> regend,
				if_false_cmd_ptr	
			);

		case RANGE_REGEX_LINE:
			return construct_regmatch_line_condition(
				jased_ctx,
				condition-> regstart,
				condition-> lineend,
				if_false_cmd_ptr	
			);
	}

	return NULL;
}

static void set_condition_if_present( optional_t condition, interpreter_ctx_t* const int_ctx ) {
    if ( !condition.is_present ) return;

    execlist_set( 
        int_ctx-> executors_list,
        int_ctx-> jased_ctx-> commands_count,
        construct_condition_executor(
            int_ctx-> jased_ctx,
            condition.cond,
            int_ctx-> jased_ctx-> commands_count + 1
       )
    );

    int_ctx-> jased_ctx-> commands_count++;
}

#define DELEGATE_PARSING_TO_FUNCTION( function_name ) \
    { \
        parser_status_t stat; \
        cqueue_getc(cqueue); \
        if ( (stat = function_name(cqueue, int_ctx)) != PARSING_OK ) return stat; \
    } break

static parser_status_t
parse_command(
        chars_queue_t* const cqueue, 
        interpreter_ctx_t* const int_ctx,
        parser_ctx_t* const parser_ctx,
        optional_t condition
) {
	char sym;
	skip_spaces(cqueue);
	sym = cqueue_gettop(cqueue);

	switch( sym ) {
		case CMD_LIST_BEGIN: {
			parser_ctx-> depth++;
			if ( condition.is_present ) {
				condition.cond_command_pointer = int_ctx-> jased_ctx-> commands_count++;
            }

			optstack_push( parser_ctx-> cond_stack, condition );
			condition.is_present = 0;
			break;
		}

		case CMD_LIST_END: {
			if ( parser_ctx-> depth != 0 ) {
				optional_t opt = optstack_pop( parser_ctx-> cond_stack );

				if ( opt.is_present ) {
                    execlist_set(
					    int_ctx-> executors_list,	
                        opt.cond_command_pointer,
						construct_condition_executor( 
							int_ctx-> jased_ctx, 
							opt.cond, int_ctx-> jased_ctx-> commands_count 
						)
                    );
				}

				parser_ctx-> depth--;

			} else return UNOPENED_CMD_LIST;

			break;
		}

		case CMD_SUB_BEGIN: 		{ set_condition_if_present( condition, int_ctx ); DELEGATE_PARSING_TO_FUNCTION( parse_sub ); }

		case CMD_TRANSLATE_BEGIN:	{ set_condition_if_present( condition, int_ctx ); DELEGATE_PARSING_TO_FUNCTION( parse_translate ); }

		case CMD_APPEND: 			{ set_condition_if_present( condition, int_ctx ); DELEGATE_PARSING_TO_FUNCTION( parse_append ); }

		case CMD_CHANGE: 			{ set_condition_if_present( condition, int_ctx ); DELEGATE_PARSING_TO_FUNCTION( parse_change ); }

		case CMD_INSERT: 			{ set_condition_if_present( condition, int_ctx ); DELEGATE_PARSING_TO_FUNCTION( parse_insert ); }
		
		case CMD_BRANCH: 			{ set_condition_if_present( condition, int_ctx ); DELEGATE_PARSING_TO_FUNCTION( parse_branch ); }

		case CMD_DELETE_PS: 		{ set_condition_if_present( condition, int_ctx ); DELEGATE_PARSING_TO_FUNCTION( parse_delete_ps ); }

		case CMD_DELETE_INIT_PS: 	{ set_condition_if_present( condition, int_ctx ); DELEGATE_PARSING_TO_FUNCTION( parse_delete_init_ps ); }

		case CMD_MOVE_HS_TO_PS: 	{ set_condition_if_present( condition, int_ctx ); DELEGATE_PARSING_TO_FUNCTION( parse_replace_hs_by_ps ); }

		case CMD_APPEND_HS_TO_PS: 	{ set_condition_if_present( condition, int_ctx ); DELEGATE_PARSING_TO_FUNCTION( parse_append_hs_to_ps ); }

		case CMD_MOVE_PS_TO_HS: 	{ set_condition_if_present( condition, int_ctx ); DELEGATE_PARSING_TO_FUNCTION( parse_replace_ps_by_hs ); }

		case CMD_APPEND_PS_TO_HS: 	{ set_condition_if_present( condition, int_ctx ); DELEGATE_PARSING_TO_FUNCTION( parse_append_ps_to_hs ); }

		case CMD_COPY_PS_TO_STDOUT:	{ set_condition_if_present( condition, int_ctx ); DELEGATE_PARSING_TO_FUNCTION( parse_n ); }

		case CMD_APPEND_LINE_TO_PS:	{ set_condition_if_present( condition, int_ctx ); DELEGATE_PARSING_TO_FUNCTION( parse_append_line_to_ps ); }

		case CMD_PRINT_PS:			{ set_condition_if_present( condition, int_ctx ); DELEGATE_PARSING_TO_FUNCTION( parse_print ); }

		case CMD_PRINTLN_PS:		{ set_condition_if_present( condition, int_ctx ); DELEGATE_PARSING_TO_FUNCTION( parse_println ); }

		case CMD_READ: 				{ set_condition_if_present( condition, int_ctx ); DELEGATE_PARSING_TO_FUNCTION( parse_read ); }

		case CMD_WRITE: 			{ set_condition_if_present( condition, int_ctx ); DELEGATE_PARSING_TO_FUNCTION( parse_write ); }

		case CMD_EXCHANGE: 			{ set_condition_if_present( condition, int_ctx ); DELEGATE_PARSING_TO_FUNCTION( parse_exchange ); }

		case CMD_PRINT_LINE_NUM: 	{ set_condition_if_present( condition, int_ctx ); DELEGATE_PARSING_TO_FUNCTION( parse_print_line_num ); }

		case CMD_LABEL: 			{ set_condition_if_present( condition, int_ctx ); DELEGATE_PARSING_TO_FUNCTION( parse_label ); }

		case CMD_QUIT: 				{ set_condition_if_present( condition, int_ctx ); DELEGATE_PARSING_TO_FUNCTION( parse_quit ); }

		default: {	
			return ILLEGAL_CHARACTER;
		}
	}

	return PARSING_OK;
}




static parser_status_t 
parse_translate( chars_queue_t* cqueue, interpreter_ctx_t* const int_ctx ) {
	char TRANSLATE_DELIMITER;
    /* backslashes count */
	size_t bs_count = 0;
    /* delimiters count (need 3) */
	size_t delim_count = 1;

	chars_queue_t* to_transform = cqueue_new( sbuffer_new() );
	chars_queue_t* transform_seq = cqueue_new( sbuffer_new() );

	TRY_GETCHAR( TRANSLATE_DELIMITER, UNTERMINATED_TRAN );

	while( !cqueue_is_empty(cqueue) ) {
		char const cur_char = cqueue_getc(cqueue);
			
		if ( cur_char == TRANSLATE_DELIMITER && bs_count % 2 == 0 ) {
			if ( ++delim_count == 3 ) {

				execlist_set(
					int_ctx-> executors_list,
					int_ctx-> jased_ctx-> commands_count++,
					construct_transform_executor(
						int_ctx-> jased_ctx,
						transformcmd,
						sbuffer_truncate( to_transform-> buffer ),
						sbuffer_truncate( transform_seq-> buffer )
					)
				); 

				cqueue_delete( to_transform );
				cqueue_delete( transform_seq );

				return PARSING_OK;
			}

			bs_count = 0; 
			continue;
		}

		if ( delim_count == 1 ) cqueue_push_back( to_transform, cur_char );
		else cqueue_push_back( transform_seq, cur_char );
			
		bs_count = cur_char == BACKSLASH ?
			bs_count + 1 : 0;
	}

	cqueue_delete( to_transform );
	cqueue_delete( transform_seq );

	return UNTERMINATED_TRAN;
}

static parser_status_t
parse_sub( chars_queue_t* const cqueue, interpreter_ctx_t* const int_ctx ) {
	char SUB_DELIMITER;
	size_t bs_count = 0;
	size_t delim_count = 1;

	chars_queue_t* regex = cqueue_new( sbuffer_new() );
	chars_queue_t* replacement = cqueue_new( sbuffer_new() );

	TRY_GETCHAR( SUB_DELIMITER, UNTERMINATED_SUB );

	while( !cqueue_is_empty(cqueue) ) {
		char const cur_char = cqueue_getc(cqueue);
			
		if ( cur_char == SUB_DELIMITER && bs_count % 2 == 0 ) {
			if ( ++delim_count == 3 ) {
				char* regex_str = sbuffer_truncate( regex-> buffer )-> char_at;
				regex_t reg;	

				if ( regcomp(&reg, regex_str, 0) ) return UNCORRECT_REGEX;

				execlist_set( 
					int_ctx-> executors_list, 
					int_ctx-> jased_ctx-> commands_count++,
					construct_regexsub_executor(
						int_ctx-> jased_ctx,
						subcmd, reg, replacement-> buffer, 0
					)
				);

				cqueue_delete( regex );
				cqueue_delete( replacement);
				return PARSING_OK;
			}

			bs_count = 0;
			continue;
		}

		if ( delim_count == 1 ) cqueue_push_back( regex, cur_char );
		else cqueue_push_back( replacement, cur_char );
			
		bs_count = cur_char == BACKSLASH ?
			bs_count + 1 : 0;
	}

    cqueue_delete( regex );
    cqueue_delete( replacement);

	return UNTERMINATED_SUB;
}

static parser_status_t
parse_append( chars_queue_t* const cqueue, interpreter_ctx_t* const int_ctx ) {
	char sym;
	chars_queue_t* append_str;

	TRY_GETCHAR( sym, UNCLOSED_APPEND );

	if ( sym != BACKSLASH ) 
		return INVALID_APPEND_SYNTAX;

    append_str = cqueue_new( sbuffer_new() );

	for ( ; ; ) {
		char cur_char;

		if ( cqueue_is_empty(cqueue) || (cur_char = cqueue_getc(cqueue)) == CMD_DELIM ) {
			execlist_set( 
				int_ctx-> executors_list,
				int_ctx-> jased_ctx-> commands_count++,
				construct_one_param_str_executor(
					int_ctx-> jased_ctx,
					append,
					sbuffer_truncate( append_str-> buffer )
				)
			);

			cqueue_delete( append_str );
			return PARSING_OK;
		}

		cqueue_push_back( append_str, cur_char );
	}
}

static parser_status_t
parse_change( chars_queue_t* const cqueue, interpreter_ctx_t* const int_ctx ) {
	char sym;
	chars_queue_t* change_str;
	TRY_GETCHAR( sym, UNCLOSED_APPEND );

	if ( sym != BACKSLASH ) 
		return INVALID_CHANGE_SYNTAX;

	change_str = cqueue_new( sbuffer_new() );

	for ( ; ; ) {
		char cur_char;

		if ( cqueue_is_empty(cqueue) || (cur_char = cqueue_getc(cqueue)) == CMD_DELIM ) {
			execlist_set( 
				int_ctx-> executors_list,
				int_ctx-> jased_ctx-> commands_count++,
				construct_one_param_str_executor(
					int_ctx-> jased_ctx,
					change,
					sbuffer_truncate( change_str-> buffer )
				)
			);

			cqueue_delete( change_str );
			return PARSING_OK;
		}

		cqueue_push_back( change_str, cur_char );
	}

	return PARSING_OK;
}

static parser_status_t 
parse_insert( chars_queue_t* const cqueue, interpreter_ctx_t* const int_ctx ) {
	char sym;
	chars_queue_t* insert_str;
	TRY_GETCHAR( sym, UNCLOSED_APPEND );

	if ( sym != BACKSLASH ) 
		return INVALID_INSERT_SYNTAX;

	insert_str = cqueue_new( sbuffer_new() );

	for ( ; ; ) {
		char cur_char;

		if ( cqueue_is_empty(cqueue) || (cur_char = cqueue_getc(cqueue)) == CMD_DELIM ) {
			execlist_set( 
				int_ctx-> executors_list,
				int_ctx-> jased_ctx-> commands_count++,
				construct_one_param_str_executor(
					int_ctx-> jased_ctx,
					change,
					sbuffer_truncate( insert_str-> buffer )
				)
			);

			cqueue_delete( insert_str );
			return PARSING_OK;
		}

		cqueue_push_back( insert_str, cur_char );
	}
		
	return PARSING_OK;

}

static parser_status_t
parse_branch( chars_queue_t* const cqueue, interpreter_ctx_t* const int_ctx ) {
	int res = skip_spaces(cqueue);
	if ( res == -1 ) return PARSING_OK;

	while ( 
		!cqueue_is_empty(cqueue) || (cqueue_getc(cqueue) == CMD_DELIM) 
	) {}

	return PARSING_OK;
}

static parser_status_t
parse_read( chars_queue_t* const cqueue, interpreter_ctx_t* const int_ctx ) {
		
	return PARSING_OK;
}


static parser_status_t
parse_write( chars_queue_t* const cqueue, interpreter_ctx_t* const int_ctx ) {
	
	return PARSING_OK;
}

static parser_status_t
parse_label( chars_queue_t* const cqueue, interpreter_ctx_t* const int_ctx ) {
	return PARSING_OK;
}

static void push_noargs_command( no_params_cmd_t cmd, interpreter_ctx_t* const int_ctx ) {
	execlist_set(
		int_ctx-> executors_list,
		int_ctx-> jased_ctx-> commands_count++, 
		construct_no_params_executor(
			int_ctx-> jased_ctx, cmd	
		)
	);
}

/* d */
DEFINE_PARSE_NOARGS_COMMAND(parse_delete_ps) {
	push_noargs_command( clear_ps, int_ctx );		
	return PARSING_OK;
}

/* D */
DEFINE_PARSE_NOARGS_COMMAND(parse_delete_init_ps) {
	push_noargs_command( delete_first_line_ps, int_ctx );		
	return PARSING_OK;
}

/* g */
DEFINE_PARSE_NOARGS_COMMAND(parse_replace_ps_by_hs) {
	push_noargs_command( copy_hs_to_ps, int_ctx );		
	return PARSING_OK;
}


/* G */
DEFINE_PARSE_NOARGS_COMMAND(parse_append_hs_to_ps) {
	push_noargs_command( append_hs_to_ps, int_ctx );		
	return PARSING_OK;
}

/* h */
DEFINE_PARSE_NOARGS_COMMAND(parse_replace_hs_by_ps) {
	push_noargs_command( copy_ps_to_hs, int_ctx );		
	return PARSING_OK;
}

/* H */
DEFINE_PARSE_NOARGS_COMMAND(parse_append_ps_to_hs) {
	push_noargs_command( append_ps_to_hs, int_ctx );		
	return PARSING_OK;
}

/* n */
DEFINE_PARSE_NOARGS_COMMAND(parse_n) {
	push_noargs_command( next, int_ctx );		
	return PARSING_OK;
}

/* N */
DEFINE_PARSE_NOARGS_COMMAND(parse_append_line_to_ps) {
	push_noargs_command( next_append, int_ctx );		
	return PARSING_OK;
}

/* p */
DEFINE_PARSE_NOARGS_COMMAND(parse_print) {
	push_noargs_command( print_ps, int_ctx );		
	return PARSING_OK;
}

/* P */
DEFINE_PARSE_NOARGS_COMMAND(parse_println) {
	push_noargs_command( print_line_ps, int_ctx );		
	return PARSING_OK;
}

/* q */
DEFINE_PARSE_NOARGS_COMMAND(parse_quit) {
	/* TODO */
	push_noargs_command( print_line_ps, int_ctx );		
	return PARSING_OK;
}

/* x */
DEFINE_PARSE_NOARGS_COMMAND(parse_exchange) {
	push_noargs_command( exchange_ps_and_hs, int_ctx );		
	return PARSING_OK;
}

/* = */
DEFINE_PARSE_NOARGS_COMMAND(parse_print_line_num) {
	push_noargs_command( print_linenum, int_ctx );		
	return PARSING_OK;
}
