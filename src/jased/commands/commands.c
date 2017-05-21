#include "jased_string_buffer.h"
#include "jased_context.h"
#include "jased_commands.h"
#include "jased_buffered_io.h"

#include <stdlib.h>

#define JASED_COMMAND_DEFINE( name ) \
RT_ERR name ( jased_ctx_t* const jased_ctx, exec_ctx_t const exec_ctx )

RT_ERR command_run( command_t* command, jased_ctx_t* const ctx ) {
	return command-> ptr( ctx, command-> cmd_ctx );
}

command_t* command_new( cmd_ptr cmd, exec_ctx_t ctx ) {
	command_t* new_command = (command_t*)malloc( sizeof(command_t) ) ;
	return 
}

void command_free( command_t* cmd ) {
	free( cmd );
}

JASED_COMMAND_DEFINE( append ) {
	/* place str on out	 */
	/* start new cycle */
}

/* n command */
JASED_COMMAND_DEFINE( next ) {
	print( jased_ctx-> pattern_space );	
	sbuffer_clear( jased_ctx-> pattern_space );

	readln( 
		jased_ctx-> stream, 
		jased_ctx-> io_buffer,
		jased_ctx-> pattern_space 
	);

}

/* N command */
JASED_COMMAND_DEFINE( next_append ) {
	string_buffer_t* buf = sbuffer_new();

	readln(
		jased_ctx-> stream,
		jased_ctx-> io_buffer,
		buf
	);

	sbuffer_append_buf(
		jased_ctx-> pattern_space, buf
	);
}


/* c command */
JASED_COMMAND_DEFINE( change ) {
	sbuffer_clear( ctx-> pattern_space );
	print( ctx-> string );	
	/* start new cycle */
}

/* = command */
JASED_COMMAND_DEFINE( print_linenum ) {
	prin( jased_ctx-> current_line );
}

/* p command */
JASED_COMMAND_DEFINE( print_ps ) {
	print( jased_ctx-> pattern_space );
}

/* P command */
JASED_COMMAND_DEFINE( print_line_ps ) {
	char* buf = jased_ctx-> pattern_space-> char_at;
	size_t i = 0;

	if ( sbuffer_is_empty(jased_ctx-> pattern_space) ) {
		return;
	}

	for ( ; i < jased_ctx-> pattern_space-> eos; i++ ) {
		if ( buf[i] == '\n' ) break;
	}

	write( jased_ctx-> stream, buf, i );
}


/* i command */
JASED_COMMAND_DEFINE( insert ) {
	print( ctx-> string );
}

JASED_COMMAND_DEFINE( branch ) {
		
}

/* w command */
JASED_COMMAND_DEFINE( write_file ) {
	
}

/* r command */
JASED_COMMAND_DEFINE( read_file ) {
	
}

/* d command */
JASED_COMMAND_DEFINE( clear_ps ) {
	sbuffer_clear( jased_ctx-> pattern_space );
}

/* D command */
JASED_COMMAND_DEFINE( delete_first_line_ps ) {
	string_buffer_t* pattern_space = jased_ctx-> pattern_space;
	size_t i = 0;	
	for ( ; i < pattern_space-> eos; i++ ) {
		if ( pattern_space-> char_at[i] == '\n' ) break;
	}

	if ( i == pattern_space-> eos ) {
		/* handle err */
		return;
	}

	sbuffer_reinit( 
		pattern_space, 
		pattern_space-> char_at + i + 1 
	);

	/* start new cycle */
}

/* g command */
JASED_COMMAND_DEFINE( copy_hs_to_ps ) {
	sbuffer_reinit( 
		jased_ctx-> pattern_space, 
		jased_ctx-> hold_space-> char_at 
	);	
}

/* G command */
JASED_COMMAND_DEFINE( append_hs_to_ps ) {
	sbuffer_append_buf( 
		jased_ctx-> patter_space, 
		jased_ctx-> hold_space 
	);
}

JASED_COMMAND_DEFINE( copy_ps_to_hs ) {
	sbuffer_reinit( 
		jased_ctx-> hold_space, 
		jased_ctx-> pattern_space-> char_at 
	);
}

JASED_COMMAND_DEFINE( append_ps_to_hs ) {
	sbuffer_append_buf( 
		jased_ctx-> hold_space, 
		jased_ctx-> pattern_space 
	);
}

JASED_COMMAND_DEFINE( exchange_ps_and_hs ) { 
	string_buffer_t* tmp = jased_ctx-> patten_space;
	jased_ctx-> pattern_space = jased_ctx-> hold_space;
	jased_ctx-> hold_space = tmp;
}
