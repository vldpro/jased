#include <string.h>

#include "jased/parser/errors.h"
#include "jased/parser/terminals.h"
#include "jased/parser/util.h"
#include "jased/parser/condition_parser.h"
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
parse_int ( chars_queue_t* const cqueue, struct condition* const condition ) {
	int result = 0;
	enum condition_type type = condition-> type;

	while ( !cqueue_is_empty(cqueue) ) {
		char const ch = cqueue_gettop(cqueue);
		if (ch < '0' || ch > '9') break;
		result = result*10 + (ch - '0');
		cqueue_getc(cqueue);
	}

	
	if ( type == LINE ) {
		condition-> linestart = result;
	} else condition-> lineend = result;

	return PARSING_OK;
}

static PARSER_STATUS 
create_regex( chars_queue_t* cqueue, struct condition* const condition ) {
	char* regex_str = sbuffer_truncate(cqueue-> buffer)-> char_at;
	enum condition_type type = condition-> type;
	regex_t reg; 


	if ( regcomp(&reg, regex_str, 0) ) {

        #ifdef DEBUG_PARSING
        #include "jased/io/io.h" 
        printerr("\t-dbg- error in string ");
        printerr( regex_str );
        printerr("\n");
        #endif
        return UNCORRECT_REGEX;
    }

	if ( type == REGEX ) {
		condition-> regstart = reg;
	} else condition-> regend = reg;

	return PARSING_OK;
}


static PARSER_STATUS 
parse_search( chars_queue_t* const cqueue, char const start_char, struct condition* const condition ) {
	size_t bs_count = 0;			
	char cur_char;
	chars_queue_t* regex = cqueue_new( sbuffer_new() );


	while ( !cqueue_is_empty(cqueue) ) {
		cur_char = cqueue_getc(cqueue);

		if ( cur_char == start_char && bs_count % 2 == 0 ) {
			return create_regex( regex, condition );
		}

		cqueue_push_back( regex, cur_char );
			
		bs_count = cur_char == BACKSLASH ? 
			bs_count + 1 : 0;
	}

	return SEARCH_EXPR_NOT_ENDED;
}

static PARSER_STATUS 
parse_cond_expr( chars_queue_t* const cqueue, struct condition* const condition ) {
	char start_char;
	if ( cqueue_is_empty(cqueue) ) return UNTERMINATED_SEARCH; 
	
	start_char = cqueue_gettop(cqueue); 

	switch( start_char ) {
		case DEFAULT_SEARCH_BEGIN: {
			if ( condition-> step == 1 ) condition-> type = REGEX;
			else condition-> type = condition-> type ==  REGEX ? 
				RANGE_REGEX : RANGE_LINE_REGEX;

			cqueue_getc(cqueue);
			return parse_search( cqueue, DEFAULT_SEARCH_BEGIN, condition );
		}

		case CUSTOM_SEARCH_BEGIN: {
			char custom_start_char;
			if ( condition-> step == 1 ) condition-> type = REGEX;
			else condition-> type = condition-> type ==  REGEX ? 
				RANGE_REGEX : RANGE_LINE_REGEX;

			cqueue_getc(cqueue);
			TRY_GETCHAR( custom_start_char, END_OF_LINE );

			return parse_search( cqueue, custom_start_char, condition );
		}

		default: {
			if ( start_char >= '0' && start_char <= '9' ) {
				if ( condition-> step == 1 ) condition-> type = LINE;
				else condition-> type = condition-> type == LINE ? 
					RANGE_LINE : RANGE_REGEX_LINE;

				return parse_int(cqueue, condition);
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
int search_delim( chars_queue_t* cqueue, struct condition* const condition ) {
	if ( cqueue_is_empty(cqueue) 
	|| ( skip_spaces(cqueue) != -1 && cqueue_gettop(cqueue) != ADDR_DELIM ) ) 
		return DELIM_NOT_FOUND;

	cqueue_getc(cqueue);
	skip_spaces(cqueue);
	condition-> step++; 
	return 0;
}

enum parser_status 
parse_condition( chars_queue_t* const cqueue, struct condition* const condition ) {
	enum state current_state = PARSING_FIRST_PART;
	condition-> step = 1;

	for(;;) {
		switch( current_state ) {
	
			case PARSING_FIRST_PART: {
				TRY( parse_cond_expr(cqueue, condition) );
				current_state = SEARCH_DELIMITER;
				break;
			}

			case SEARCH_DELIMITER: {
				current_state = search_delim(cqueue, condition) == DELIM_NOT_FOUND ?
					PARSING_END : PARSING_SECOND_PART;
				
				break;
			}
	
			case PARSING_SECOND_PART: {
				TRY( parse_cond_expr(cqueue, condition) );
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
