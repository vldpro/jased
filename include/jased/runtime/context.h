#ifndef JASED_CONTEXT
#define JASED_CONTEXT

#include <stddef.h> 
#include "jased/util/string_buffer.h"
#include "jased/util/hashmap.h"
#include "jased/io/io.h"

#define LABELS_DEFAULT_SIZE 32 

typedef struct jased_ctx {
    /* sed hold and pattern space */
	string_buffer_t* pattern_space;
	string_buffer_t* hold_space;

    /* the lines which prints after/before printing pattern space */
    string_buffer_t* after;
    string_buffer_t* before;

    /* hashmap for sed labels */
    hashmap_t* labels;

    /* number of line, which handling by interpeter */
	size_t current_line;
    /* current command pointer */
	size_t command_pointer;

    /* in and out streams for commands */
	int in_stream;
	int out_stream;

	int is_new_cycle_enable;
	int is_default_output_enable;
    int is_any_subs_matched;

    /* count of parsed commands */
	int commands_count;

    string_buffer_t* sub_wfile;

	io_buffer_t* io_buffer;
} jased_ctx_t;


jased_ctx_t* jased_ctx_new();

void jased_ctx_delete( jased_ctx_t* ctx );

#endif
