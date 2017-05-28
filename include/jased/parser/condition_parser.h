#ifndef JASED_ADDR_PARSER
#define JASED_ADDR_PARSER

#include <stddef.h>
#include "jased/runtime/executors.h"
#include "jased/util/chars_queue.h"
#include "jased/parser/errors.h"

struct condition {
    /* start of condition (regex or num) */
	regex_t regstart;
	size_t linestart;

    /* end of condition (regex or num) */
	regex_t regend;
	size_t lineend;

    /* type of condition */
	enum condition_type {
		LINE, RANGE_LINE, REGEX, RANGE_REGEX,
		RANGE_LINE_REGEX, RANGE_REGEX_LINE
	} type;

    /* translation step (only for condition parser) */
	size_t step;
};

enum parser_status
parse_condition( chars_queue_t* cqueue, struct condition* const addr_obj );

#endif
