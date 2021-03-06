#ifndef PARSER_ERR
#define PARSER_ERR

typedef enum parser_status {
	PARSING_OK = 0,
	UNTERMINATED_SUB,
	UNTERMINATED_TRAN,

	INVALID_APPEND_SYNTAX,
	UNCLOSED_APPEND,

	INVALID_CHANGE_SYNTAX,
	INVALID_INSERT_SYNTAX,

	UNTERMINATED_SEARCH,

	SEARCH_EXPR_NOT_ENDED,
	END_OF_LINE,
	ILLEGAL_CHARACTER,

	UNCORRECT_REGEX,
	UNOPENED_CMD_LIST,
	UNCLOSED_CMD_LIST,
	EMPTY_FILE,
    EMPTY_LABEL,
    LABEL_DOUBLE_DEFINITION,
    UNCORRECT_SEARCH_DELIMITER,
    UNCORRECT_SUB_DELIMITER,
    UNCORRECT_TRANSLATE_DELIMITER,
    EMPTY_FILENAME
} parser_status_t;

char* const get_errmsg( parser_status_t status );

#endif
