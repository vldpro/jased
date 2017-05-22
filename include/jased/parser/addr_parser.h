#ifndef JASED_ADDR_PARSER
#define JASED_ADDR_PARSER

#include <stddef.h>
#include "jased/runtime/executors.h"
#include "jased/util/chars_queue.h"
#include "jased/parser/errors.h"

struct address {
	regex_t regstart;
	size_t linestart;

	regex_t regend;
	size_t lineend;

	enum addr_type {
		LINE, RANGE_LINE, REGEX, RANGE_REGEX,
		RANGE_LINE_REGEX, RANGE_REGEX_LINE
	} type;
};

enum parser_status
parse_addr( chars_queue_t* cqueue, struct address* const addr_obj );

#endif
