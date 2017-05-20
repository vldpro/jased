#ifndef PARSER_TERMINALS 
#define PARSER_TERMINALS 

/* Sed terminal symbols */

typedef enum terminal_char {
	BACKSLASH = '\\',
	COMMENT = '#',

	CMD_SUB_BEGIN = 's',
	CMD_SUB_DEFAULT_DELIM = '/',

	CMD_TRANSLATE_BEGIN = 'y',

	DEFAULT_SEARCH_BEGIN = '/',
	DEFAULT_SEARCH_END = '/',

	CUSTOM_SEARCH_BEGIN = '\\',

	ADDR_DELIM = ',',

	CMD_DELIM = ';',	
	CMD_DELIM_NEWLINE = '\n',
	CMD_LIST_BEGIN = '{',
	CMD_LIST_END = '}',

	CMD_INSERT = 'i',
	CMD_APPEND = 'a',
	CMD_CHANGE = 'c',
	CMD_BRANCH = 'b',

	CMD_DELETE_PS = 'd',
	CMD_DELETE_INIT_PS = 'D',

	CMD_MOVE_HS_TO_PS = 'g',
	CMD_APPEND_HS_TO_PS = 'G',

	CMD_MOVE_PS_TO_HS = 'h',
	CMD_APPEND_PS_TO_HS = 'H',

	CMD_COPY_PS_TO_STDOUT = 'n',
	CMD_APPEND_LINE_TO_PS = 'N',

	CMD_PRINT_PS = 'p',
	CMD_PRINTLN_PS = 'P',
	CMD_QUIT = 'q',

	CMD_READ = 'r',
	CMD_WRITE = 'w',

	CMD_EXCHANGE = 'x',

	CMD_NEG = '!',	
	CMD_LABEL = ':',
	CMD_PRINT_LINE_NUM = '='

} terminal_char_t;

#endif
