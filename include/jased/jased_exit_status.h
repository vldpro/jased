#ifndef JASED_EXIT_STATUS
#define JASED_EXIT_STATUS

enum jased_exit_status {
    ERROR_IN_CMDLINE_ARGS = 1,
    ERROR_IN_OPEN_FILES,
    ERROR_IN_PARSING,
    ERROR_IO,
    ERROR_OOM
};

#endif
