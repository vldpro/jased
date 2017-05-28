#include "jased/parser/errors.h"

char* const get_errmsg( parser_status_t status ) {
    switch( status ) {
        case UNTERMINATED_SUB: return "Parsing error: unterminated 's' command.\n";

        case UNTERMINATED_TRAN: return "Parsing error: unterminated 'y' command.\n";

        case INVALID_APPEND_SYNTAX: return "Parsing error: invalid syntax for 'a' command.\n";

        case UNCLOSED_APPEND: return "Parsing error: unclosed 'a' command.\n";

        case INVALID_CHANGE_SYNTAX: return "Parsing error: invalid syntax for'c' command.\n";

        case INVALID_INSERT_SYNTAX: return "Parsing error: invalid syntax for'i' command.\n";

        case UNTERMINATED_SEARCH: return "Parsing error: unterminated search (/ or \\c) command.\n";

        case UNCORRECT_REGEX: return "Parsing error: invalid syntax for regular expression.\n";

        case UNOPENED_CMD_LIST: return "Parsing error: commands list close, but not started.\n";

        case UNCLOSED_CMD_LIST: return "Parsing error: unclosed commands list.\n";

        case EMPTY_FILE: return "Parsing error: empty file.\n";

        case ILLEGAL_CHARACTER: return "Parsing error: illegal command.\n";

        default: return "Internal error: Oops... this kind of error is not declared by developer.\n";
    }

}
