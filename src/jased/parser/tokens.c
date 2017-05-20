#include <string.h>
#include <stdio.h>

#include "jased_tokens.h"
#include "jased_io.h"

#define PARSER_STATUS enum parse_status

static int is_space( char const sym ) {
	return sym == ' ' || sym == '\t';	
}

static size_t skip_spaces( char const* string ) {	
	size_t i = 0;
	while ( string[i] != '\0' && is_space(string[i++]) );

	return i - 1;
}

PARSER_STATUS sub_cmd_parser( char const* line, size_t const length );

PARSER_STATUS parse( char const* line, size_t const length ) {
	size_t cur_char = skip_spaces( line );
	size_t offset = cur_char + 1;
	char sym = line[cur_char];
	puts("IN PARSER");

	printf("%c\n", line[cur_char]);

	if ( line[cur_char] == '\0' ) 
		return PARSED_OK;

	switch( sym ) {
		case CMD_SUB_BEGIN:
			return tr_and_sub_cmd_parser( line + offset, length );
			break;

		case CMD_TRANSLATE_BEGIN:
			return tr_and_sub_cmd_parser( line + offset, length );
			break;

		case CUSTOM_SEARCH_BEGIN:
			return parse_custom_search( line + offset, length );
			break;

		case DEFAULT_SEARCH_BEGIN:
			return parse_default_search( line + offset, length );

		case CMD_LIST_BEGIN:
			return PARSED_OK;
			break;
		default:
			return UNDEFINED_TOKEN;
	}
	
}

int is_number( char const sym ) {
	return sym > '0' && sym < '9';
}

PARSER_STATUS  parse_address( char const* line, size_t const length ) {
	size_t cur_char = skip_spaces( line );
	size_t offset = cur_char + 1;

	char sym = line[cur_char];

	switch( sym ) {
		case DEFAULT_SEARCH_BEGIN:
			break;

		case CUSTOM_SEARCH_BEGIN:
			break;

		default:	
			if ( is_number(sym) ) 
			break;
	}
	
}

#define UNDEFINED -1

size_t parse_operand( char const* line, size_t const length, char const end_sym ) {
	size_t i = 0;
	size_t chieldchars_count = 0;
		
	for ( ; i < length; i++ ) {
		if ( line[i] == end_sym && (chieldchars_count == 0 || chieldchars % 2 != 0 ) )
				break;

		chieldchars_count = line[i] == CMD_SHIELD_CHAR ? 
			chieldchars + 1 : 0;
		
	}

	return i;
}

PARSER_STATUS parse_default_search( char const* line, size_t const length ) {
	size_t offset = parse_operand( line , length, DEFAULT_SEARCH_END );

	return offset == length ? DEFAULT_SEARCH_ERROR : PARSED_OK;
}

PARSER_STATUS parse_custom_search( char const* line, size_t const length ) {
	char const SEARCH_BEG_END = line[0];

	size_t offset = parse_operand( line + 1, length, SEARCH_BEG_END );	

	return offset == length ? CUSTOM_SEARCH_ERROR : PARSED_OK;
}

PARSER_STATUS tr_and_sub_cmd_parser( char const* line, size_t const length ) {
	char const SUB_DELIM = line[0]; 
	size_t delim_count = 0;
	size_t i = 1;

	int regex_start = UNDEFINED;
	int regex_end = UNDEFINED;

	int replacement_start = UNDEFINED;
	int replacement_end = UNDEFINED;

	puts("IN SUB PARSER");

	
	for ( ; i < length; i++ ) {
		if ( line[i] == SUB_DELIM ) {
			if ( delim_count == 0 ) {
				puts("FIND FIRST DELIMITER");
				regex_start = 0; 
				regex_end = i - 1;
				replacement_start = i + 1;
				delim_count++;

			} else if ( delim_count == 1 ) {
				puts("FIND SECOND DELIMITER");
				replacement_end = i - 1;	
				delim_count++;
				
			} else break;
		}
	}

	if ( regex_start != UNDEFINED && replacement_end != UNDEFINED ) {
		puts(" SUB PARSED OK");
		/* TODO */
		regex_end++;
		replacement_start++;
		parse( 
			line + replacement_end + 2, 
			length - replacement_end - 2 
		);

		return PARSED_OK;
	} 
		
	return SUB_PARSE_ERROR;
}

int main( int argc, char** argv ) {
	string_buffer_t* str = sbuffer_new();	
	io_buffer_t* io = io_buffer_new();

	readln( STDIN_FILENO, io, str );
	puts(str-> char_at);
	return parse( str-> char_at, str-> eos );
}
