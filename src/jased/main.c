#include <string.h> 

enum command_line_args_err {
	NO_ARGUMENTS = 1,
	INVALID_ARGS,
};

int main( int argc, char** argv ) {
	if ( argc == 0 ) {
		return NO_ARGUMENTS;	
	} else if ( argc == 1 ) {
		
		
	} else {
		size_t i = 0;
		for ( ; i < argc; i++ ) {
			if ( !strcmp(argv[i], "-c") ) {

			} else if ( !strcmp[i], "-f" ) {

			} else if ( !strcmp[i], "-n" ) {

			} else {

			}
		}
	}

}
