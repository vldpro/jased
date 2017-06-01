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

#define DEFINE_ONE_INT_PARAM_CMD( name ) \
RT_ERR name( jased_ctx_t* const jased_ctx, int const val ) 

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
        jased_ctx-> is_any_subs_matched = 1;
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
        jased_ctx-> is_any_subs_matched = 1;
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
        jased_ctx-> is_any_subs_matched = 1;
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
DEFINE_ONE_INT_PARAM_CMD( write_file ) { 
    print( val, jased_ctx-> pattern_space );
    return 0; 
}

/* r command */
DEFINE_ONE_STRING_PARAM_CMD( read_file ) { 
    int fd = open( str-> char_at, O_RDONLY );
    string_buffer_t* line;
    io_buffer_t* iobuf;

    if ( fd == -1 ) return 0;

    line = sbuffer_new();
    iobuf = io_buffer_new();

    while ( readln(fd, iobuf, line) != -1 ) {
        print( jased_ctx-> out_stream, line ); 
    }

    sbuffer_delete( line );
    io_buffer_delete( iobuf );

    return 0; 
}

DEFINE_ONE_STRING_PARAM_CMD( branch ) { 
    int command = hmap_get( jased_ctx-> labels, str-> char_at );

    if ( str-> eos == 0 ) {
        jased_ctx-> command_pointer = jased_ctx-> commands_count - 2;
        return 0;
    }

    if ( command == HMAP_UNDEFINED ) {
        printerr("jased: undefined label '");
        printerr( str-> char_at );
        printerr("'\n");
        return UNDEFINED_LABEL_FOR_BRANCH;
    }

    jased_ctx-> command_pointer = command - 1;

    return 0; 
}


DEFINE_ONE_STRING_PARAM_CMD( test ) { 
    int command;

    if ( str-> eos == 0 ) {
        jased_ctx-> command_pointer = jased_ctx-> commands_count - 2;
        return 0;
    }
    
    if ( jased_ctx-> is_any_subs_matched == 0 ) return 0;

    command = hmap_get( jased_ctx-> labels, str-> char_at );

    if ( command == HMAP_UNDEFINED ) {
        printerr("jased: undefined label '");
        printerr( str-> char_at );
        printerr("'\n");
        return UNDEFINED_LABEL_FOR_BRANCH;
    }

    jased_ctx-> command_pointer = command - 1;

    return 0;
}

/* i command */
DEFINE_ONE_STRING_PARAM_CMD( insert ) {
    if ( !jased_ctx-> is_default_output_enable ) {
	    print( jased_ctx-> out_stream, str );
        print_stream( jased_ctx-> out_stream, "\n" );

    } else {
        string_buffer_t* tmp = sbuffer_clone(jased_ctx-> pattern_space);
        sbuffer_clear(jased_ctx-> pattern_space);
        sbuffer_append_buf(
            jased_ctx-> pattern_space, str
        );

        sbuffer_append_char(jased_ctx-> pattern_space, '\n');

        sbuffer_append_buf(
            jased_ctx-> pattern_space, tmp
        );

        sbuffer_delete(tmp);
    }

	/* jased_ctx-> is_new_cycle_enable = 1; */


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

	if ( res == -1 ) return quit(jased_ctx);

	sbuffer_append_buf(
		jased_ctx-> pattern_space, buf
	);

    jased_ctx-> current_line++;

	return 0;
}

/* = command */
DEFINE_NO_PARAMS_CMD( print_linenum ) {
    print_int( jased_ctx-> out_stream, jased_ctx-> current_line );
    print_stream( jased_ctx-> out_stream, "\n");
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

    /* TODO write */
	write( jased_ctx-> out_stream, buf, i + 1 );
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

/* g command */
DEFINE_NO_PARAMS_CMD( copy_hs_to_ps ) {
    sbuffer_reinit( 
            jased_ctx-> pattern_space, 
            jased_ctx-> hold_space-> char_at
    );

    return 0;
}

/* H command */
DEFINE_NO_PARAMS_CMD( append_ps_to_hs ) {
	sbuffer_append_buf( 
		jased_ctx-> hold_space, 
		jased_ctx-> pattern_space 
	);

	return 0;
}

/* h command */
DEFINE_NO_PARAMS_CMD( copy_ps_to_hs ) {
    sbuffer_clear( jased_ctx-> hold_space );
	return append_ps_to_hs( jased_ctx );
}

DEFINE_NO_PARAMS_CMD( exchange_ps_and_hs ) { 
	string_buffer_t* tmp = jased_ctx-> pattern_space;
	jased_ctx-> pattern_space = jased_ctx-> hold_space;
	jased_ctx-> hold_space = tmp;

	return 0;
}
