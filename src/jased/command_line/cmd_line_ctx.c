#include <unistd.h>
#include <stddef.h> 
#include <malloc.h>

#include "jased/command_line/cmd_line_ctx.h"
#include "jased/io/io.h"

#define DEFAULT_COUNT 10 

int getopt(int argc, char *argv[], const char *optstring);
extern char *optarg;
extern int optind, opterr, optopt;

enum jased_options {
    SCRIPT_KEY = 'e',
    SCRIPTFILE_KEY = 'f',
    DEFAULT_OUTPUT_KEY = 'n'
};

void cmdline_ctx_delete( struct cmdline_ctx* ctx) {
    free( ctx-> script_filenames );
    free( ctx-> args_scripts );
    free( ctx );
}

struct cmdline_ctx*
get_cmdline_ctx( int const argc, char** argv ) {
    char* const cmdline_options = "ne:f:";
    struct cmdline_ctx* ctx = malloc( sizeof(struct cmdline_ctx) );
    int opt;

    ctx-> script_filenames = malloc( sizeof(char*) * DEFAULT_COUNT );
    ctx-> args_scripts =     malloc( sizeof(char*) * DEFAULT_COUNT );
    ctx-> source_filenames = NULL;

    ctx-> source_files_count = 0;
    ctx-> script_files_count = 0;
    ctx-> args_scripts_count = 0;
    ctx-> is_default_output_enable = 1;

    if ( argc == 1 ) {
       println("jased syntax: jased [-n] [-e script ]... [ -f scriptfile ]... [ sourcefile ]...");
       cmdline_ctx_delete( ctx );
       return NULL;
    }


    opterr = 1; /* getopt handle errors */
    while ( (opt = getopt(argc, argv, cmdline_options)) != -1 ) {
        switch(opt) {
            case SCRIPT_KEY: {
                if ( ctx-> args_scripts_count == DEFAULT_COUNT ) {
                    cmdline_ctx_delete( ctx ); 
                    printerr("jased: too many arguments scripts");
                    return NULL;
                }

                ctx-> args_scripts[ ctx-> args_scripts_count ] = optarg;
                ctx-> args_scripts_count++;
                break;
            }

            case SCRIPTFILE_KEY: {
                if ( ctx-> script_files_count == DEFAULT_COUNT ) {
                    cmdline_ctx_delete( ctx ); 
                    printerr("jased: too many arguments file scripts");
                    return NULL;
                }

                ctx-> script_filenames[ctx-> script_files_count] =  optarg; 
                ctx-> script_files_count++;
                break;
            }

            case DEFAULT_OUTPUT_KEY: {
                ctx-> is_default_output_enable = 0;
                break;
            }
        }
    }

    ctx-> source_filenames = argv + optind; 
    ctx-> source_files_count = argc - optind;

    return ctx;
}
