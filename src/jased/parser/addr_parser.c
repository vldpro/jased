#include <string.h>
#include <stdio.h>

#include "jased/parser/tokens.h"
#include "jased/parser/util.h"
#include "jased/parser/addr_parser.h"

#include "jased/util/chars_queue.h"


#define TRY_GETCHAR( dest, on_fail ) \
do { \
	if ( cqueue_is_empty() ) (on_fail); \
	(dest) = cqueue_getc(cqueue); \
} while(0)


enum state {
	PARSING_FIRST_PART,
	SEARCH_DELIMITER,
	PARSING_SECOND_PART,
	PARSING_END,
	/* err states */

	SEARCH_EXPR_NOT_ENDED,
};

#define PARSER_STATUS enum state 

static PARSER_STATUS
parse_int ( chars_queue_t* const cqueue ) {
	bool minus = false;
	int result = 0;
	char ch;
	ch = cqueue_getc(cqueue);

	while (true) {
		if (ch == '-') break;
		if (ch >= '0' && ch <= '9') break;
		ch = cqueue_getc(cqueue);
	}

	if (ch == '-') minus = true; else result = ch - '0';

	while (true) {
		ch = cqueue_getc(cqueue);
		if (ch < '0' || ch > '9') break;
		result = result*10 + (ch - '0');
	}

	result = minus ? -result : result;

	return PARSING_OK;
}


static PARSER_STATUS 
parse_search( chars_queue_t* const cqueue, char const start_char ) {
	#define BACKSLASH '\'
	size_t bs_count = 0;			
	char cur_char;

	while ( !cqueue_is_empty(cqueue) ) {
		cur_char = cqueue_getc(cqueue);

		if ( cur_char == start_char && bs_count % 2 == 0 ) {
			return PARSING_OK;
		}
			
		bs_count = cur_char == BACKSLASH ? 
			bs_count + 1 : 0;
	}

	return SEARCH_EXPRESSION_NOT_ENDED;
}

static PARSER_STATUS 
parse_addr_expr( chars_queue_t* const cqueue ) {
	char start_char;
	TRY_GETCHAR( start_char, return END_OF_LINE );

	switch( start_char ) {
		case DEFAULT_SEARCH_BEGIN: 
			return parse_search( cqueue, DEFAULT_SEARCH_BEGIN );

		case CUSTOM_SEARCH_BEGIN:
			char custom_start_char;
			TRY_GETCHAR( start_char, return END_OF_LINE );

			return parse_search( cqueue, custom_start_char );

		default: 
			return parse_int();
	}
}

#define TRY( action ) \
do { \
	PARSER_STATUS stat; \
	if ( (stat = (action)) != PARSING_OK )	\
		return stat; \
} while(0)
	

enum parser_status 
parse_addr( chars_queue_t* const cqueue ) {
	enum state current_state = PARSING_FIRST_PART;

	for(;;) {
		switch( current_state ) {
	
			case PARSING_FIRST_PART:
				TRY( parse_addr_expr(cqueue) );
				current_state = SEARCH_DELIMITER;
				break;

			case SEARCH_DELIMITER:
				current_state = search_delim(cqueue) == DELIM_NOT_FOUND ?
					PARSING_END : PARSING_SECOND_PART;
				
				break;
	
			case PARSING_SECOND_PART:
				TRY( parse_addr_expr(cqueue) );
				current_state = PARSING_END;
				break;

			case PARSING_END:
				return PARSING_OK;

			default:
				return SEARCH_EXPR_NOT_ENDED;

		}
	}
}


