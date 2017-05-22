#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "jased/parser/util.h"
#include "jased/parser/terminals.h"
#include "jased/parser/cmd_parser.h"
#include "jased/parser/errors.h"
#include "jased/util/chars_queue.h"
#include "jased/io/io.h"

#define PARSER_STATUS parser_status_t

#define PARSE_NOARGS_COMMAND(name) \
static PARSER_STATUS \
name( chars_queue_t* const cqueue, interpreter_ctx_t* const int_ctx )

#define TRY_GETCHAR( dest, err_code ) \
	do {  \
		if ( !cqueue_is_empty(cqueue) ) dest = cqueue_getc(cqueue); \
		else return err_code; \
	} while(0)

static PARSER_STATUS 
parse_translate( chars_queue_t* cqueue, interpreter_ctx_t* const int_ctx ) {
	char TRANSLATE_DELIMITER;
	size_t bs_count = 0;
	size_t delim_count = 1;

	chars_queue_t* to_transform = cqueue_new( sbuffer_new() );
	chars_queue_t* transform_seq = cqueue_new( sbuffer_new() );

	TRY_GETCHAR( TRANSLATE_DELIMITER, UNTERMINATED_SUB );

	while( !cqueue_is_empty(cqueue) ) {
		char const cur_char = cqueue_getc(cqueue);
			
		if ( cur_char == TRANSLATE_DELIMITER && bs_count % 2 == 0 ) {
			if ( ++delim_count == 3 ) {
				execlist_push_back(
					int_ctx-> jased_ctx,
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

	puts("here");

	return UNTERMINATED_SUB;
}

static PARSER_STATUS
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
				char* regex_str = sbuffer_truncate( regex )-> char_at;
				regex_t reg;	

				if ( regcomp(&reg, regex_str, 0) ) return UNCORRECT_REGEX;
					
				execlist_push_back( 
					int_ctx-> executors_list, 
					construct_regsub_executor(
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

	return UNTERMINATED_SUB;
}

static PARSER_STATUS
parse_append( chars_queue_t* const cqueue, interpreter_ctx_t* const int_ctx ) {
	char sym;
	chars_queue_t* append_str = cqueue_new( sbuffer_new() );
	TRY_GETCHAR( sym, UNCLOSED_APPEND );

	if ( sym != BACKSLASH ) 
		return INVALID_APPEND_SYNTAX;

	for ( ; ; ) {
		char const cur_char;

		if ( cqueue_is_empty(cqueue) || (cur_char = cqueue_getc(cqueue)) == CMD_DELIM ) {
			execlist_push_back( 
				int_ctx-> executors_list,
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

static PARSER_STATUS
parse_change( chars_queue_t* const cqueue ) {
	char sym;
	chars_queue_t* change_str = cqueue_new( sbuffer_new() );
	TRY_GETCHAR( sym, UNCLOSED_APPEND );

	if ( sym != BACKSLASH ) 
		return INVALID_CHANGE_SYNTAX;

	for ( ; ; ) {
		char const cur_char;

		if ( cqueue_is_empty(cqueue) || (cur_char = cqueue_getc(cqueue)) == CMD_DELIM ) {
			execlist_push_back( 
				int_ctx-> executors_list,
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

static PARSER_STATUS 
parse_insert( chars_queue_t* const cqueue ) {
	char sym;
	chars_queue_t* insert_str = cqueue_new( sbuffer_new() );
	TRY_GETCHAR( sym, UNCLOSED_APPEND );

	if ( sym != BACKSLASH ) 
		return INVALID_INSERT_SYNTAX;

	for ( ; ; ) {
		char const cur_char;

		if ( cqueue_is_empty(cqueue) || (cur_char = cqueue_getc(cqueue)) == CMD_DELIM ) {
			execlist_push_back( 
				int_ctx-> executors_list,
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

static PARSER_STATUS
parse_branch( chars_queue_t* const cqueue ) {
	int res = skip_spaces(cqueue);
	if ( res == -1 ) return PARSING_OK;

	while ( 
		!cqueue_is_empty(cqueue) || (cqueue_getc(cqueue) == CMD_DELIM) 
	) {}

	return PARSING_OK;
}

static PARSER_STATUS
parse_read( chars_queue_t* const cqueue ) {
		
	return PARSING_OK;
}


static PARSER_STATUS
parse_write( chars_queue_t* const cqueue ) {
	
	return PARSING_OK;
}

static PARSER_STATUS
parse_label( chars_queue_t* const cqueue ) {
	return PARSING_OK;
}

static void push_noargs_command( no_params_cmd_t cmd, interpreter_ctx_t* const int_ctx ) {
	execlist_push_back(
		int_ctx-> executors_list,
		construct_no_params_executor(
			int_ctx-> jased_ctx, cmd	
		)
	);
}



/* d */
PARSE_NOARGS_COMMAND(parse_delete_ps) {
	push_noargs_command( clear_ps, int_ctx );		
	return PARSING_OK;
}

/* D */
PARSE_NOARGS_COMMAND(parse_delete_init_ps) {
	push_noargs_command( delete_first_line_ps, int_ctx );		
	return PARSING_OK;
}

/* g */
PARSE_NOARGS_COMMAND(parse_replace_ps_by_hs) {
	push_noargs_command( copy_hs_to_ps, int_ctx );		
	return PARSING_OK;
}


/* G */
PARSE_NOARGS_COMMAND(parse_append_hs_to_ps) {
	push_noargs_command( append_hs_to_ps, int_ctx );		
	return PARSING_OK;
}

/* h */
PARSE_NOARGS_COMMAND(parse_replace_hs_by_ps) {
	push_noargs_command( copy_ps_to_hs, int_ctx );		
	return PARSING_OK;
}

/* H */
PARSE_NOARGS_COMMAND(parse_append_ps_to_hs) {
	push_noargs_command( append_ps_to_hs, int_ctx );		
	return PARSING_OK;
}

/* n */
PARSE_NOARGS_COMMAND(parse_n) {
	push_noargs_command( next, int_ctx );		
	return PARSING_OK;
}

/* N */
PARSE_NOARGS_COMMAND(parse_append_line_to_ps) {
	push_noargs_command( next_append, int_ctx );		
	return PARSING_OK;
}

/* p */
PARSE_NOARGS_COMMAND(parse_print) {
	push_noargs_command( print_ps, int_ctx );		
	return PARSING_OK;
}

/* P */
PARSE_NOARGS_COMMAND(parse_println) {
	push_noargs_command( print_line_ps, int_ctx );		
	return PARSING_OK;
}

/* q */
PARSE_NOARGS_COMMAND(parse_quit) {
	/* TODO */
	push_noargs_command( print_line_ps, int_ctx );		
	return PARSING_OK;
}

/* x */
PARSE_NOARGS_COMMAND(parse_exchange) {
	push_noargs_command( exchange_ps_and_hs, int_ctx );		
	return PARSING_OK;
}

/* = */
PARSE_NOARGS_COMMAND(parse_print_line_num) {
	push_noargs_command( print_linenum, int_ctx );		
	return PARSING_OK;
}

static executor_t* 
create_cond_executor( 
	jased_ctx_t* const jased_ctx, 
	struct address* const addr_obj, 
	int const if_false_cmd_ptr 
) {
	enum addr_type type = addr_obj-> type;	

	switch( type ) {
		case LINE:
			return construct_line_condition( 
				jased_ctx,
				addr_obj-> linestart,
				jased_ctx-> commands + 2
			);

		case RANGE_LINE:
			return construct_line_range_condition(
				jased_ctx,
				addr_obj-> linestart,
				addr_obj-> lineend,
				if_false_cmd_ptr	
			);

		case REGEX:
			return construct_regmatch_condition(
				jased_ctx,
				addr_obj-> regstart,
				if_false_cmd_ptr	
			);

		case RANGE_REGEX:
			return construct_regmatch_range_condition(
				jased_ctx,
				addr_obj-> regstart,
				addr_obj-> regend,
			    if_false_cmd_ptr	
			);

		case RANGE_LINE_REGEX:
			return construct_line_regmatch_condition(
				jased_ctx,
				addr_obj-> linestart,
				addr_obj-> regend,
				if_false_cmd_ptr	
			);

		case RANGE_REGEX_LINE:
			return construct_regmatch_line_condition(
				jased_ctx,
				addr_obj-> regstart,
				addr_obj-> lineend,
				if_false_cmd_ptr	
			);
	}
}

#define TRY_READLN( on_fail ) \
	do { \
		if ( !readln( \
				int_ctx-> jased_ctx-> stream, \
				int_ctx-> jased_ctx-> io_buffer, \
				str \
		) ){ \
			on_fail \
		cqueue = cqueue_new(str); \
	} while(0)
	

PARSER_STATUS parse( 
	interpreter_ctx_t* const int_ctx 
	struct parser_context* const parser_ctx
) {
	#define DELEGATE_TO( handler ) \
	do { \
		PARSER_STATUS stat; \
		if ( current_address.is_present ) { \
			int_ctx-> executors_list[ int_ctx-> jased_ctx-> commads ] = \
				create_cond_executor( int_ctx-> jased_ctx, current_address.addr )	\
			int_ctx-> jased_ctx-> commands++; \
			current_address.is_present = 0;\
		} \
		\
		cqueue_getc(cqueue); \
		\
		if( (stat = handler(cqueue, int_ctx)) != PARSING_OK )  \
			return stat; \
		int_ctx-> jased_ctx-> commands++; \
	} while(0); break \

	chars_queue_t* cqueue;

	struct optional current_address;
	current_address.addr = malloc( sizeof(struct address) );
	current_address.is_present = 0;

	TRY_READLN( { return EMPTY_FILE; } );
	
	for( ; ; ) {
		char sym;

		if ( cqueue_is_empty(cqueue) ) {
			TRY_READLN( { 
				if ( parser_ctx-> addr_stack-> top != -1 ) {
					return INCLOSED_CMD_LIST;
				}
				return PARSING_OK;
			} );
		}

		skip_spaces(cqueue);
		sym = cqueue_gettop(cqueue);

		switch( sym ) {
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

			case CUSTOM_SEARCH_BEGIN:
			case DEFAULT_SEARCH_BEGIN: {
				parse_addr( cqueue, current_address-> addr );
				current_address-> is_present = 1;
				break;
			}

			case CMD_LIST_BEGIN: {
				parser_ctx-> depth++;
				if ( current_address.is_present ) 
					current_address.position = int_ctx-> jased_ctx-> commands++;
				addrstack_push( parser_ctx-> addrs, current_address );
			}

			case CMD_LIST_END: {
				if ( parser_ctx-> depth != 0 ) {
					struct optional opt = addrstack_pop( parser_ctx-> addrs );
					if ( opt.is_present ) {
						int_ctx-> executors_list[ opt.position ] =	
							create_cond_executor( 
								int_ctx-> jased_ctx, 
								opt.addr, opt.position
							);
					}
					return PARSING_OK;

				} else return UNOPENED_CMD_LIST;
			}

			case CMD_SUB_BEGIN: 		DELEGATE_TO(parse_sub);

			case CMD_TRANSLATE_BEGIN:	DELEGATE_TO(parse_translate);

			case CMD_APPEND: 			DELEGATE_TO(parse_append);

			case CMD_CHANGE: 			DELEGATE_TO(parse_change);

			case CMD_INSERT: 			DELEGATE_TO(parse_insert);
			
			case CMD_BRANCH: 			DELEGATE_TO(parse_branch);

			case CMD_DELETE_PS: 		DELEGATE_TO(parse_delete_ps);

			case CMD_DELETE_INIT_PS: 	DELEGATE_TO(parse_delete_init_ps);

			case CMD_MOVE_HS_TO_PS: 	DELEGATE_TO(parse_replace_hs_by_ps);

			case CMD_APPEND_HS_TO_PS: 	DELEGATE_TO(parse_append_hs_to_ps);

			case CMD_MOVE_PS_TO_HS: 	DELEGATE_TO(parse_replace_ps_by_hs);

			case CMD_APPEND_PS_TO_HS: 	DELEGATE_TO(parse_append_ps_to_hs);

			case CMD_COPY_PS_TO_STDOUT:	DELEGATE_TO(parse_n);

			case CMD_APPEND_LINE_TO_PS:	DELEGATE_TO(parse_append_line_to_ps);

			case CMD_PRINT_PS:			DELEGATE_TO(parse_print);

			case CMD_PRINTLN_PS:		DELEGATE_TO(parse_println);

			case CMD_READ: 				DELEGATE_TO(parse_read);

			case CMD_WRITE: 			DELEGATE_TO(parse_write);

			case CMD_EXCHANGE: 			DELEGATE_TO(parse_exchange);

			case CMD_PRINT_LINE_NUM: 	DELEGATE_TO(parse_print_line_num);

			case CMD_LABEL: 			DELEGATE_TO(parse_label);

			case CMD_QUIT: 				DELEGATE_TO(parse_quit);

			default: {	
				if ( sym >= '0' && sym <= '9' ) parse_addr(cqueue);
				else return ILLEGAL_CHARACTER;
			}

		}
	}

	return PARSING_OK;
}

int main( int argc, char** argv ) {
	string_buffer_t* str = sbuffer_new();
	io_buffer_t* iobuf = io_buffer_new();
	chars_queue_t* cqueue;

	readln( STDIN_FILENO, iobuf, str );
	cqueue = cqueue_new(str);

	return parse_line(cqueue);
}
