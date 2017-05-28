#ifndef CMD_LINE_CTX
#define CMD_LINE_CTX

struct cmdline_ctx {
    int is_default_output_enable;

    size_t script_files_count;
    size_t args_scripts_count;
    size_t source_files_count;

    char** script_filenames;
    char** source_filenames;
    char** args_scripts;
};

struct cmdline_ctx* get_cmdline_ctx( int const argv, char** const argc );

void cmdline_ctx_delete( struct cmdline_ctx* ctx );

#endif
