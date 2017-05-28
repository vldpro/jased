#include <stdlib.h>

#include "jased/util/string_buffer.h"
#include "jased/commands/commands.h"
#include "jased/commands/regex.h"
#include "jased/io/io.h"

#define RT_ERR int

#define DEFINE_NO_PARAMS_CMD( name ) \
RT_ERR name( jased_ctx_t* const jased_ctx )

#define DEFINE_ONE_STRING_PARAM_CMD( name ) \
RT_ERR name( jased_ctx_t* const jased_ctx, string_buffer_t* const str ) 

#define DEFINE_REGSUB_CMD( name ) \
RT_ERR name( jased_ctx_t* const jased_ctx, \
        regex_t const regex, \
        string_buffer_t* const str, \
        int const flags, int const match_num, int const wfile )

#define DEFINE_TRANSFORM_CMD( name ) \
RT_ERR name( jased_ctx_t* const jased_ctx, string_buffer_t* to_transform, string_buffer_t* transform_seq )

DEFINE_REGSUB_CMD( subcmd ) {
	int matched = sub( 
		jased_ctx-> pattern_space,
		regex, str, flags
	);

    if ( matched ) {
        if ( IS_FLAG_ENABLE(flags, P_FLAG) ) {
            print_ps( jased_ctx );

        } else if ( IS_FLAG_ENABLE(flags, PINIT_FLAG) ) {
            print_line_ps( jased_ctx );
        }

        if ( IS_FLAG_ENABLE(flags, W_FLAG) ) {
            print( wfile, jased_ctx-> pattern_space );
        }
    }

	return 0;
}

DEFINE_REGSUB_CMD( gsubcmd ) {
	int matched = gsub(
		jased_ctx-> pattern_space,
		regex, str, flags
	);

    if ( matched ) {
        if ( IS_FLAG_ENABLE(flags, P_FLAG) ) {
            print_ps( jased_ctx );

        } else if ( IS_FLAG_ENABLE(flags, PINIT_FLAG) ) {
            print_line_ps( jased_ctx );

        }

        if ( IS_FLAG_ENABLE(flags, W_FLAG) ) {
            print( wfile, jased_ctx-> pattern_space );
        }
    }

	return 0;
}

DEFINE_REGSUB_CMD( nsubcmd ) {
	int matched = nsub(
		jased_ctx-> pattern_space,
		regex, str, match_num 
	);

    if ( matched ) {
        if ( IS_FLAG_ENABLE(flags, P_FLAG) ) {
            print_ps( jased_ctx );

        } else if ( IS_FLAG_ENABLE(flags, PINIT_FLAG) ) {
            print_line_ps( jased_ctx );
        }

        if ( IS_FLAG_ENABLE(flags, W_FLAG) ) {
            print( wfile, jased_ctx-> pattern_space );
        }
    }

	return 0;
}

DEFINE_TRANSFORM_CMD( transformcmd ) {
	transform(
		jased_ctx-> pattern_space-> char_at,
		to_transform-> char_at,
		transform_seq-> char_at
	);	

	return 0;
}

DEFINE_NO_PARAMS_CMD(empty_cmd) {
    return 0;
}

DEFINE_ONE_STRING_PARAM_CMD( append ) {
	if ( jased_ctx-> is_default_output_enable ) {
		print( jased_ctx-> out_stream, jased_ctx-> pattern_space );
	}

	print( jased_ctx-> out_stream, str );
	jased_ctx-> is_new_cycle_enable = 1;

	return 0;
}

/* w command */
DEFINE_ONE_STRING_PARAM_CMD( write_file ) { return 0; }

/* r command */
DEFINE_ONE_STRING_PARAM_CMD( read_file ) { return 0; }

DEFINE_ONE_STRING_PARAM_CMD( branch ) { return 0; }

/* i command */
DEFINE_ONE_STRING_PARAM_CMD( insert ) {
	print( jased_ctx-> out_stream, str );
	jased_ctx-> is_new_cycle_enable = 1;

	if ( jased_ctx-> is_default_output_enable ) {
		print( jased_ctx-> out_stream, jased_ctx-> pattern_space );
	}

	return 0;
}

/* c command */
DEFINE_ONE_STRING_PARAM_CMD( change ) {
	sbuffer_clear( jased_ctx-> pattern_space );
	
	print( jased_ctx-> out_stream, str );
	jased_ctx-> is_new_cycle_enable = 1;

	return 0;
}

/* d command */
DEFINE_NO_PARAMS_CMD( clear_ps ) {
	sbuffer_clear( jased_ctx-> pattern_space );
	return 0;
}

/* D command */
DEFINE_NO_PARAMS_CMD( delete_first_line_ps ) {
	string_buffer_t* pattern_space = jased_ctx-> pattern_space;
	size_t i = 0;	
	for ( ; i < pattern_space-> eos; i++ ) {
		if ( pattern_space-> char_at[i] == '\n' ) break;
	}

	if ( i == pattern_space-> eos ) {
		sbuffer_clear( jased_ctx-> pattern_space );
	}

	sbuffer_reinit( 
		pattern_space, 
		pattern_space-> char_at + i + 1 
	);

	jased_ctx-> is_new_cycle_enable = 1;

	return 0;
}

/* n command */
DEFINE_NO_PARAMS_CMD( next ) {
	if ( jased_ctx-> is_default_output_enable )
		print( jased_ctx-> out_stream, jased_ctx-> pattern_space );

	sbuffer_clear( jased_ctx-> pattern_space );

	readln( 
		jased_ctx-> in_stream, 
		jased_ctx-> io_buffer,
		jased_ctx-> pattern_space 
	);

	return 0;
}

/* q command */
DEFINE_NO_PARAMS_CMD( quit ) {
	return 1;
}

/* N command */
DEFINE_NO_PARAMS_CMD( next_append ) {
	string_buffer_t* buf = sbuffer_new();

	ssize_t res = readln(
		jased_ctx-> in_stream,
		jased_ctx-> io_buffer,
		buf
	);

	if ( res == -1 ) return 1;

	sbuffer_append_buf(
		buf, jased_ctx-> pattern_space
	);

	sbuffer_delete( jased_ctx-> pattern_space );

	jased_ctx-> pattern_space = buf;	

	return 0;
}

/* = command */
DEFINE_NO_PARAMS_CMD( print_linenum ) {
	/* print(  jased_ctx-> out_stream, jased_ctx-> current_line ); */
	return 0;
}

/* p command */
DEFINE_NO_PARAMS_CMD( print_ps ) {
	print( jased_ctx-> out_stream, jased_ctx-> pattern_space );
	return 0;
}

/* P command */
DEFINE_NO_PARAMS_CMD( print_line_ps ) {
	char* buf = jased_ctx-> pattern_space-> char_at;
	size_t i = 0;

	for ( ; i < jased_ctx-> pattern_space-> eos; i++ ) {
		if ( buf[i] == '\n' ) break;
	}

	write( jased_ctx-> out_stream, buf, i );
	return 0;
}

/* g command */
DEFINE_NO_PARAMS_CMD( copy_hs_to_ps ) {
	sbuffer_reinit( 
		jased_ctx-> pattern_space, 
		jased_ctx-> hold_space-> char_at 
	);	

	return 0;
}


/* G command */
DEFINE_NO_PARAMS_CMD( append_hs_to_ps ) {
	sbuffer_append_buf( 
		jased_ctx-> pattern_space, 
		jased_ctx-> hold_space 
	);

	return 0;
}

DEFINE_NO_PARAMS_CMD( copy_ps_to_hs ) {
	sbuffer_reinit( 
		jased_ctx-> hold_space, 
		jased_ctx-> pattern_space-> char_at 
	);

	return 0;
}

DEFINE_NO_PARAMS_CMD( append_ps_to_hs ) {
	sbuffer_append_buf( 
		jased_ctx-> hold_space, 
		jased_ctx-> pattern_space 
	);

	return 0;
}

DEFINE_NO_PARAMS_CMD( exchange_ps_and_hs ) { 
	string_buffer_t* tmp = jased_ctx-> pattern_space;
	jased_ctx-> pattern_space = jased_ctx-> hold_space;
	jased_ctx-> hold_space = tmp;

	return 0;
}
