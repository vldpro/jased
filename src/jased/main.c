#include <string.h> 
#include <malloc.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>

#include "jased/jased_exit_status.h"
#include "jased/runtime/interpreter.h"

#include "jased/parser/errors.h"
#include "jased/parser/jased_parser.h"
#include "jased/io/io.h"
#include "jased/command_line/cmd_line_ctx.h"

/* free all memory */
static void delete( struct cmdline_ctx* cmdline_ctx, interpreter_ctx_t** contexts, int const ctxs_count ) {
    size_t i = 0;
    cmdline_ctx_delete( cmdline_ctx ); 

    for ( ; i < ctxs_count; i++ ) {
        interpreter_ctx_delete( contexts[i] );
    }

    free( contexts );
}

#define CHECK_OPEN( fd, filename ) \
   if ( fd == -1 ) { \
       int errnum = errno; \
       printerr("jased: can not open file or directory '"); \
       printerr( filename ); \
       printerr( "'. "); \
       printerr( strerror(errnum) ); \
       printerr("\n"); \
       delete( cmdline_ctx, int_contexts, total_scripts_count ); \
       return ERROR_IN_OPEN_FILES; \
   }

/* check parsing exval and if error occured - write errmsg to STDERR */
static int is_parsing_ok( parser_exit_status_t parser_exval ) {
    if ( parser_exval.is_ok ) return 1;

    printerr( get_errmsg(parser_exval.parser_status) );

    printerr( "Parsing stopped. (line " );
    print_int( STDERR_FILENO, parser_exval.stop_on_line );

    printerr( ", sym " );
    print_int( STDERR_FILENO, parser_exval.stop_on_symbol );
    printerr(")\n");

    return 0;
}

int main( int argc, char** argv ) {
    struct cmdline_ctx* cmdline_ctx = get_cmdline_ctx( argc, argv );
    size_t total_scripts_count;
    interpreter_ctx_t** int_contexts;
    size_t i = 0;

    #ifdef DEBUG_PARSING
    printerr( "jased: compiled in mode DEBUG_PARSING\n\n" );
    #endif

    if ( cmdline_ctx == NULL ) { 
        return ERROR_IN_CMDLINE_ARGS;
    }

    total_scripts_count = 
        cmdline_ctx-> script_files_count + cmdline_ctx-> args_scripts_count;

    int_contexts = (interpreter_ctx_t**)malloc( sizeof(interpreter_ctx_t*) * total_scripts_count );

    /* create contexts */
    for ( i = 0; i < total_scripts_count; i++ ) {
        int_contexts[i] = interpreter_ctx_new(); 
        int_contexts[i]-> jased_ctx-> is_default_output_enable =
            cmdline_ctx-> is_default_output_enable;
    }

    /* parse script files */
    for ( i = 0; i < cmdline_ctx-> script_files_count; i++ ) {
        int file = open_file( cmdline_ctx-> script_filenames[i], O_RDONLY );

        if ( !is_parsing_ok(parse_file(file, int_contexts[i])) ) {
            delete( cmdline_ctx, int_contexts, total_scripts_count );
            close( file );
            return ERROR_IN_PARSING;
        }

        close_file( file );
    }

    /* parse commandline scripts */
    for ( i = 0; i < cmdline_ctx-> args_scripts_count; i++ ) {
        parser_exit_status_t parser_exval = parse_commandline_script( 
                cmdline_ctx-> args_scripts[i], 
                int_contexts[ i + cmdline_ctx-> script_files_count ]
        );

        if ( !is_parsing_ok(parser_exval) ) {
            delete( cmdline_ctx, int_contexts, total_scripts_count );
            return ERROR_IN_PARSING;
        }
    }

    /* run all parsed scripts on all source files */
    for ( i = 0; i < cmdline_ctx-> source_files_count; i++ ) {
        int source_file = open_file( cmdline_ctx-> source_filenames[i], O_RDONLY );

        run( source_file, int_contexts, total_scripts_count ); 

        close_file( source_file );
    }

    /* if no source files, read stdin */
    if ( i == 0 ) {
        run( STDIN_FILENO, int_contexts, total_scripts_count );
    }

    delete( cmdline_ctx, int_contexts, total_scripts_count );

	return EXIT_SUCCESS;
}
