#ifndef PARSER_ERR
#define PARSER_ERR

typedef enum parser_status {
	PARSING_OK = 0,
	UNTERMINATED_SUB,

	INVALID_APPEND_SYNTAX,
	UNCLOSED_APPEND,

	INVALID_CHANGE_SYNTAX,
	INVALID_INSERT_SYNTAX,

	UNTERMINATED_SEARCH,

	SEARCH_EXPR_NOT_ENDED,
	END_OF_LINE,
	ILLEGAL_CHARACTER
} parser_status_t;

#endif
