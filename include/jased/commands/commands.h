#ifndef JASED_COMMANDS
#define JASED_COMMANDS

#include "jased/util/string_buffer.h"
#include "jased/runtime/context.h"
#include "jased/commands/regex.h"

#include "stdlib.h"

typedef enum sub_flags {
    N_FLAG = 1,
    P_FLAG = 2,
    W_FLAG = 4,
    PINIT_FLAG = 8,
    G_FLAG = 16 
} sub_flags_t;

/* commands exit code */
#define COMPLETED_SUCCESSFULLY 0
#define QUIT_COMMAND 1
#define UNDEFINED_LABEL_FOR_BRANCH 2



#define IS_FLAG_ENABLE( src, flag ) ((src) & (flag)) != 0

#define DECLARE_NO_PARAMS_CMD( name ) \
int name( jased_ctx_t* const jased_ctx )

#define DECLARE_ONE_STRING_PARAM_CMD( name ) \
int name( jased_ctx_t* const jased_ctx, string_buffer_t* const str ) 


#define DECLARE_ONE_INT_PARAM_CMD( name ) \
int name( jased_ctx_t* const jased_ctx, int const val ) 

#define DECLARE_REGSUB_CMD( name ) \
int name( jased_ctx_t* const jased_ctx,  \
        regex_t const regex, \
        string_buffer_t* const str, \
        int const flags, int const num, int const wfile  ) \

#define DECLARE_TRANSFORM_CMD( name ) \
int name( jased_ctx_t* const jased_ctx, string_buffer_t* to_transform, string_buffer_t* transform_seq )


DECLARE_REGSUB_CMD( subcmd );
DECLARE_REGSUB_CMD( gsubcmd );
DECLARE_REGSUB_CMD( nsubcmd );

DECLARE_TRANSFORM_CMD( transformcmd );

DECLARE_ONE_STRING_PARAM_CMD( append );
DECLARE_ONE_STRING_PARAM_CMD( insert );
DECLARE_ONE_STRING_PARAM_CMD( change );
DECLARE_ONE_STRING_PARAM_CMD( read_file );
DECLARE_ONE_STRING_PARAM_CMD( branch );
DECLARE_ONE_STRING_PARAM_CMD( test );

DECLARE_ONE_INT_PARAM_CMD( write_file );

DECLARE_NO_PARAMS_CMD( empty_cmd );
DECLARE_NO_PARAMS_CMD( clear_ps );
DECLARE_NO_PARAMS_CMD( delete_first_line_ps );
DECLARE_NO_PARAMS_CMD( next );
DECLARE_NO_PARAMS_CMD( next_append );
DECLARE_NO_PARAMS_CMD( print_linenum );
DECLARE_NO_PARAMS_CMD( print_ps );
DECLARE_NO_PARAMS_CMD( print_line_ps );
DECLARE_NO_PARAMS_CMD( copy_hs_to_ps );
DECLARE_NO_PARAMS_CMD( append_hs_to_ps );
DECLARE_NO_PARAMS_CMD( copy_ps_to_hs );
DECLARE_NO_PARAMS_CMD( append_ps_to_hs );
DECLARE_NO_PARAMS_CMD( exchange_ps_and_hs );
DECLARE_NO_PARAMS_CMD( quit );

#endif
