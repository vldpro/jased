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

static void handle_flags( int const flags, jased_ctx_t* const jased_ctx, int const wfile ) {
        if ( IS_FLAG_ENABLE(flags, P_FLAG) ) {
            print_ps( jased_ctx );

        } else if ( IS_FLAG_ENABLE(flags, PINIT_FLAG) ) {
            print_line_ps( jased_ctx );
        }

        /* write ps to file */
        if ( IS_FLAG_ENABLE(flags, W_FLAG) ) {
            print( wfile, jased_ctx-> pattern_space );
        }
}

static int run_subtitution(
        int (*substitution) (
                string_buffer_t* const, regex_t const, 
                string_buffer_t* const, int const
        ),

        jased_ctx_t* const jased_ctx,
        regex_t const regex,
        string_buffer_t* const str,
        int const flags, int const match_num, int const wfile, int const sub_arg
) {

    int result = substitution( 
		jased_ctx-> pattern_space,
		regex, str, sub_arg 
	);

    if ( (jased_ctx-> is_any_subs_matched = result) == MATCH ) {
        handle_flags( flags, jased_ctx, wfile ); 
    }

	return COMPLETED_SUCCESSFULLY;
}

DEFINE_REGSUB_CMD( subcmd ) {
	return run_subtitution(
            sub, jased_ctx, regex, str, flags, match_num, wfile, flags 
    ); 
}

DEFINE_REGSUB_CMD( gsubcmd ) {
	return run_subtitution(
            gsub, jased_ctx, regex, str, flags, match_num, wfile, flags 
    );
}

DEFINE_REGSUB_CMD( nsubcmd ) {
	return run_subtitution(
            nsub, jased_ctx, regex, str, flags, match_num, wfile, match_num 
    );
}

DEFINE_TRANSFORM_CMD( transformcmd ) {
	transform(
		jased_ctx-> pattern_space-> char_at,
		to_transform-> char_at,
		transform_seq-> char_at
	);	

	return COMPLETED_SUCCESSFULLY;
}

DEFINE_NO_PARAMS_CMD(empty_cmd) {
    return COMPLETED_SUCCESSFULLY;
}

/* a command */
DEFINE_ONE_STRING_PARAM_CMD( append ) {
    sbuffer_append_buf(
        jased_ctx-> after, str
    );

    sbuffer_append_char( jased_ctx-> after, '\n' );

	return COMPLETED_SUCCESSFULLY;
}

/* i command */
DEFINE_ONE_STRING_PARAM_CMD( insert ) {
    sbuffer_append_buf(
        jased_ctx-> print_buffer, str
    );

    sbuffer_append_char( jased_ctx-> print_buffer, '\n' );

	return COMPLETED_SUCCESSFULLY;
}

/* c command */
DEFINE_ONE_STRING_PARAM_CMD( change ) {
	sbuffer_clear( jased_ctx-> pattern_space );

    sbuffer_append_buf(
        jased_ctx-> print_buffer, str
    );

    sbuffer_append_char( jased_ctx-> print_buffer, '\n' );
	
	/* print( jased_ctx-> out_stream, str ); */
	jased_ctx-> is_new_cycle_enable = 1;

	return COMPLETED_SUCCESSFULLY;
}

/* w command */
DEFINE_ONE_INT_PARAM_CMD( write_file ) { 
    print( val, jased_ctx-> pattern_space );
    return COMPLETED_SUCCESSFULLY; 
}

/* r command */
DEFINE_ONE_STRING_PARAM_CMD( read_file ) { 
    int fd = open( str-> char_at, O_RDONLY );
    string_buffer_t* line;
    io_buffer_t* iobuf;

    if ( fd == -1 ) return COMPLETED_SUCCESSFULLY;

    line = sbuffer_new();
    iobuf = io_buffer_new();

    while ( readln(fd, iobuf, line) != -1 ) {
        sbuffer_append_buf( 
            jased_ctx-> print_buffer, 
            line 
        ); 
    }

    sbuffer_delete( line );
    io_buffer_delete( iobuf );

    return COMPLETED_SUCCESSFULLY; 
}


/* b command */
DEFINE_ONE_STRING_PARAM_CMD( branch ) { 
    int command = hmap_get( jased_ctx-> labels, str-> char_at );

    if ( str-> eos == 0 ) {
        jased_ctx-> command_pointer = jased_ctx-> commands_count - 2;
        return COMPLETED_SUCCESSFULLY;
    }

    if ( command == HMAP_UNDEFINED ) {
        printerr("jased: undefined label '");
        printerr( str-> char_at );
        printerr("'\n");
        return UNDEFINED_LABEL_FOR_BRANCH;
    }

    jased_ctx-> command_pointer = command - 1;

    return COMPLETED_SUCCESSFULLY; 
}


/* t command */
DEFINE_ONE_STRING_PARAM_CMD( test ) { 
    int command= 0;

    if ( str-> eos == 0 ) {
        jased_ctx-> command_pointer = jased_ctx-> commands_count - 2;
        return 0;
    }
    
    if ( !jased_ctx-> is_any_subs_matched ) return COMPLETED_SUCCESSFULLY;
    jased_ctx-> is_any_subs_matched = 0;

    /*printerr("branch to ");
    printerr( str-> char_at );
    printerr("\n");*/

    command = hmap_get( jased_ctx-> labels, str-> char_at );

    if ( command == HMAP_UNDEFINED ) {
        printerr("jased: undefined label '");
        printerr( str-> char_at );
        printerr("'\n");
        return UNDEFINED_LABEL_FOR_BRANCH;
    }

    jased_ctx-> command_pointer = command - 1;

    return COMPLETED_SUCCESSFULLY;
}




/* d command */
DEFINE_NO_PARAMS_CMD( clear_ps ) {
	sbuffer_clear( jased_ctx-> pattern_space );
	return COMPLETED_SUCCESSFULLY;
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

	return COMPLETED_SUCCESSFULLY;
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

	return COMPLETED_SUCCESSFULLY;
}

/* q command */
DEFINE_NO_PARAMS_CMD( quit ) {
	return QUIT_COMMAND;
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

	return COMPLETED_SUCCESSFULLY;
}

/* = command */
DEFINE_NO_PARAMS_CMD( print_linenum ) {
    print_int( jased_ctx-> out_stream, jased_ctx-> current_line );
    print_stream( jased_ctx-> out_stream, "\n");

	return COMPLETED_SUCCESSFULLY;
}

/* p command */
DEFINE_NO_PARAMS_CMD( print_ps ) {
	sbuffer_append_buf( 
        jased_ctx-> print_buffer, 
        jased_ctx-> pattern_space 
    );

	return COMPLETED_SUCCESSFULLY;
}

/* P command */
DEFINE_NO_PARAMS_CMD( print_line_ps ) {
	char* buf = jased_ctx-> pattern_space-> char_at;
	size_t i = 0;

	for ( ; i < jased_ctx-> pattern_space-> eos; i++ ) {
		if ( buf[i] == '\n' ) break;
	}

    /* TODO write */
	sbuffer_append( 
        jased_ctx-> print_buffer, 
        buf, i + 1 
    );

	return COMPLETED_SUCCESSFULLY;
}


/* G command */
DEFINE_NO_PARAMS_CMD( append_hs_to_ps ) {
	sbuffer_append_buf( 
		jased_ctx-> pattern_space, 
		jased_ctx-> hold_space 
	);

	return COMPLETED_SUCCESSFULLY;
}

/* g command */
DEFINE_NO_PARAMS_CMD( copy_hs_to_ps ) {
    sbuffer_reinit( 
            jased_ctx-> pattern_space, 
            jased_ctx-> hold_space-> char_at
    );

    return COMPLETED_SUCCESSFULLY;
}

/* H command */
DEFINE_NO_PARAMS_CMD( append_ps_to_hs ) {
	sbuffer_append_buf( 
		jased_ctx-> hold_space, 
		jased_ctx-> pattern_space 
	);

	return COMPLETED_SUCCESSFULLY;
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

	return COMPLETED_SUCCESSFULLY;
}
