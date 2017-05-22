#include <string.h>
#include <stdio.h>

#include "jased/parser/errors.h"
#include "jased/parser/terminals.h"
#include "jased/parser/util.h"
#include "jased/util/chars_queue.h"

#define PARSER_STATUS parser_status_t

#define TRY_GETCHAR( dest, on_fail ) \
do { \
	if ( cqueue_is_empty(cqueue) ) return on_fail; \
	dest = cqueue_getc(cqueue); \
} while(0)


enum state {
	PARSING_FIRST_PART,
	SEARCH_DELIMITER,
	PARSING_SECOND_PART,
	PARSING_END
};

static PARSER_STATUS
parse_int ( chars_queue_t* const cqueue, struct address* const addr_obj ) {
	int result = 0;

	while ( !cqueue_is_empty(cqueue) ) {
		char const ch = cqueue_gettop(cqueue);
		if (ch < '0' || ch > '9') break;
		result = result*10 + (ch - '0');
		cqueue_getc(cqueue);
	}

	if ( type == LINE ) {
		addr_obj-> linestart = result;
	} else addr_obj-> lineend = result;

	return PARSING_OK;
}

static PARSER_STATUS 
create_regex( chars_queue_t* cqueue, struct address* const addr_obj ) {
	char* regex_str = sbuffer_truncate(cqueue-> buffer)-> char_at;
	enum addr_type type = addr_obj-> type;
	regex_t reg; 

	if ( !regcomp( &reg, regex_str, 0 ) ) return UNCORRECT_REGEX;

	if ( type == REGEX ) {
		addr_obj-> regstart = reg;
	} else addr_obj-> regend = reg;

	return PARSING_OK;
}


static PARSER_STATUS 
parse_search( chars_queue_t* const cqueue, char const start_char, struct address* const addr_obj ) {
	size_t bs_count = 0;			
	char cur_char;
	chars_queue_t* regex;


	while ( !cqueue_is_empty(cqueue) ) {
		cur_char = cqueue_getc(cqueue);

		if ( cur_char == start_char && bs_count % 2 == 0 ) {
			return create_regex( regex, addr_obj );
		}

		cqueue_push_back( regex, cur_char );
			
		bs_count = cur_char == BACKSLASH ? 
			bs_count + 1 : 0;
	}

	return SEARCH_EXPR_NOT_ENDED;
}

static PARSER_STATUS 
parse_addr_expr( chars_queue_t* const cqueue, struct address* const addr_obj ) {
	char start_char;
	if ( cqueue_is_empty(cqueue) ) return UNTERMINATED_SEARCH; 
	
	start_char = cqueue_gettop(cqueue); 

	switch( start_char ) {
		case DEFAULT_SEARCH_BEGIN: {
			if ( addr_obj-> step == 1 ) addr_obj-> type = REGEX;
			else addr_obj-> type = addr_obj-> type ==  REGEX ? 
				RANGE_REGEX : RANGE_LINE_REGEX;

			cqueue_getc(cqueue);
			return parse_search( cqueue, DEFAULT_SEARCH_BEGIN, addr_obj );
		}

		case CUSTOM_SEARCH_BEGIN: {
			char custom_start_char;
			if ( addr_obj-> step == 1 ) addr_obj-> type = REGEX;
			else addr_obj-> type = addr_obj-> type ==  REGEX ? 
				RANGE_REGEX : RANGE_LINE_REGEX;

			cqueue_getc(cqueue);
			TRY_GETCHAR( custom_start_char, END_OF_LINE );

			return parse_search( cqueue, custom_start_char, addr_obj );
		}

		default: {
			if ( start_char >= '0' && start_char <= '9' ) {
				if ( step == 1 ) addr_obj-> type = LINE;
				else addr_obj-> type = addr_obj-> type == LINE ? 
					RANGE_LINE : RANGE_REGEX_LINE;

				return parse_int(cqueue, addr_obj);
			} else return UNTERMINATED_SEARCH;
		}
	}

	return PARSING_OK;
}

#define TRY( action ) \
do { \
	PARSER_STATUS stat; \
	if ( (stat = (action)) != PARSING_OK )	\
		return stat; \
} while(0)

#define DELIM_NOT_FOUND -1
int search_delim( chars_queue_t* cqueue, struct address* const addr_obj ) {
	if ( cqueue_is_empty(cqueue) 
	|| ( skip_spaces(cqueue) != -1 && cqueue_gettop(cqueue) != ADDR_DELIM ) ) 
		return DELIM_NOT_FOUND;

	cqueue_getc(cqueue);
	skip_spaces(cqueue);
	addr_obj-> step++; 
	return 0;
}

enum parser_status 
parse_addr( chars_queue_t* const cqueue, struct address* const addr_obj ) {
	enum state current_state = PARSING_FIRST_PART;

	for(;;) {
		switch( current_state ) {
	
			case PARSING_FIRST_PART: {
				TRY( parse_addr_expr(cqueue, addr_obj) );
				current_state = SEARCH_DELIMITER;
				break;
			}

			case SEARCH_DELIMITER: {
				current_state = search_delim(cqueue, addr_obj) == DELIM_NOT_FOUND ?
					PARSING_END : PARSING_SECOND_PART;
				
				break;
			}
	
			case PARSING_SECOND_PART: {
				TRY( parse_addr_expr(cqueue, addr_obj) );
				current_state = PARSING_END;
				break;
			}

			case PARSING_END: {
				return PARSING_OK;
			}

			default: {
				return SEARCH_EXPR_NOT_ENDED;
			}

		}
	}
}
