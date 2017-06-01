#ifndef JASED_CONDITIONS
#define JASED_CONDITIONS

#include <regex.h>
#include <stddef.h>

#include "jased/runtime/context.h"
#define RT_ERR int

struct line_condition_args {
	size_t line;
    int is_negative;

	int if_false_cmd_ptr;
};

struct lines_range_condition_args {
	size_t start, end;

    int is_negative;
	int if_false_cmd_ptr;
};

struct regmatch_condition_args {
	regex_t regex;

    int is_negative;
	int if_false_cmd_ptr;
};

struct regmatch_range_condition_args {
	regex_t start;
	int is_start_matched;

	regex_t end;
	int is_end_matched;

    int is_negative;
	int if_false_cmd_ptr;
};

struct line_regmatch_range_condition_args {
	size_t start;

	regex_t end;
	int is_end_matched;

    int is_negative;
	int if_false_cmd_ptr;
};

struct regmatch_line_range_condition_args {
	regex_t start;
	int is_start_matched;

	size_t end;

    int is_negative;
	int if_false_cmd_ptr;
};

typedef union condition_args {
	void*  args;
	struct line_condition_args*        line_cond_args;
	struct lines_range_condition_args* lines_range_cond_args;
	struct regmatch_condition_args*    regmatch_cond_args;

	struct regmatch_range_condition_args* 	   regmatch_range_cond_args;
	struct line_regmatch_range_condition_args* line_regmatch_range_cond_args;
	struct regmatch_line_range_condition_args* regmatch_line_range_cond_args;
} condition_args_t;

condition_args_t* condition_args_new();

#endif
