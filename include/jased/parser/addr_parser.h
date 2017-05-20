#ifndef JASED_ADDR_PARSER
#define JASED_ADDR_PARSER

#include <stddef.h>
#include "jased/util/chars_queue.h"
#include "jased/parser/errors.h"

enum parser_status
parse_addr( chars_queue_t* cqueue);

#endif
