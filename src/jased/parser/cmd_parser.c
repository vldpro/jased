#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "jased/parser/util.h"
#include "jased/parser/terminals.h"
#include "jased/parser/addr_parser.h"
#include "jased/parser/errors.h"
#include "jased/util/chars_queue.h"
#include "jased/io/io.h"

#define PARSER_STATUS parser_status_t

#define PARSE_COMMAND(name) \
static PARSER_STATUS \
name( chars_queue_t* const cqueue )

#define TRY_GETCHAR( dest, err_code ) \
	do {  \
		if ( !cqueue_is_empty(cqueue) ) dest = cqueue_getc(cqueue); \
		else return err_code; \
	} while(0)

static PARSER_STATUS 
parse_translate( chars_queue_t* cqueue ) {
	char TRANSLATE_DELIMITER;
	size_t bs_count = 0;
	size_t delim_count = 1;

	TRY_GETCHAR( TRANSLATE_DELIMITER, UNTERMINATED_SUB );

	while( !cqueue_is_empty(cqueue) ) {
		char const cur_char = cqueue_getc(cqueue);
			
		if ( cur_char == TRANSLATE_DELIMITER && bs_count % 2 == 0 ) {
			if ( ++delim_count == 3 ) return PARSING_OK;
		}
			
		bs_count = cur_char == BACKSLASH ?
			bs_count + 1 : 0;
	}

	puts("here");

	return UNTERMINATED_SUB;

}

static PARSER_STATUS
parse_sub( chars_queue_t* const cqueue ) {
	char SUB_DELIMITER;
	size_t bs_count = 0;
	size_t delim_count = 1;

	TRY_GETCHAR( SUB_DELIMITER, UNTERMINATED_SUB );

	while( !cqueue_is_empty(cqueue) ) {
		char const cur_char = cqueue_getc(cqueue);
			
		if ( cur_char == SUB_DELIMITER && bs_count % 2 == 0 ) {
			if ( ++delim_count == 3 ) return PARSING_OK;
		}
			
		bs_count = cur_char == BACKSLASH ?
			bs_count + 1 : 0;
	}

	puts("here");

	return UNTERMINATED_SUB;
}

static PARSER_STATUS
parse_append( chars_queue_t* const cqueue ) {
	char sym;
	TRY_GETCHAR( sym, UNCLOSED_APPEND );

	if ( sym != BACKSLASH ) 
		return INVALID_APPEND_SYNTAX;

	while ( 
		!cqueue_is_empty(cqueue) || (cqueue_getc(cqueue) == CMD_DELIM) 
	) {}
	
	return PARSING_OK;
}

static PARSER_STATUS
parse_change( chars_queue_t* const cqueue ) {
	char sym;
	TRY_GETCHAR( sym, UNCLOSED_APPEND );

	if ( sym != BACKSLASH ) 
		return INVALID_CHANGE_SYNTAX;

	while ( 
		!cqueue_is_empty(cqueue) || (cqueue_getc(cqueue) == CMD_DELIM) 
	) {}
	
	return PARSING_OK;
}

static PARSER_STATUS 
parse_insert( chars_queue_t* const cqueue ) {
	char sym;
	TRY_GETCHAR( sym, UNCLOSED_APPEND );

	if ( sym != BACKSLASH ) 
		return INVALID_INSERT_SYNTAX;

	while ( 
		!cqueue_is_empty(cqueue) || (cqueue_getc(cqueue) == CMD_DELIM) 
	) {}
	
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

/* d */
PARSE_COMMAND(parse_delete_ps) {
	return PARSING_OK;
}

/* D */
PARSE_COMMAND(parse_delete_init_ps) {
	return PARSING_OK;
}

/* g */
PARSE_COMMAND(parse_replace_ps_by_hs) {
	return PARSING_OK;
}


/* G */
PARSE_COMMAND(parse_append_hs_to_ps) {
	return PARSING_OK;
}

/* h */
PARSE_COMMAND(parse_replace_hs_by_ps) {
	return PARSING_OK;
}

/* H */
PARSE_COMMAND(parse_append_ps_to_hs) {
	return PARSING_OK;
}

/* n */
PARSE_COMMAND(parse_n) {
	return PARSING_OK;
}

/* N */
PARSE_COMMAND(parse_append_line_to_ps) {
	return PARSING_OK;
}

/* p */
PARSE_COMMAND(parse_print) {
	return PARSING_OK;
}

/* P */
PARSE_COMMAND(parse_println) {
	return PARSING_OK;
}

/* q */
PARSE_COMMAND(parse_quit) {
	return PARSING_OK;
}

/* x */
PARSE_COMMAND(parse_exchange) {
	return PARSING_OK;
}

/* = */
PARSE_COMMAND(parse_print_line_num) {
	return PARSING_OK;
}

PARSER_STATUS parse_line( chars_queue_t* const cqueue ) {
	#define DELEGATE_TO( handler ) \
	do { \
		PARSER_STATUS stat; \
		cqueue_getc(cqueue); \
		if( (stat = handler(cqueue)) != PARSING_OK )  \
			return stat; \
	} while(0); break \
	
	while( !cqueue_is_empty(cqueue) ) {
		char sym;
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
			case DEFAULT_SEARCH_BEGIN: 
				parse_addr( cqueue );
				break;

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
