#include <unistd.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <stdio.h>

#include <jased/commands/regex.h>

#define REVERSE_LINK_LEN 2

int match( char const* const string, regex_t const regexp ){
	return regexec( &regexp, string, 0, NULL, 0);
}

static int insert_subexpr_matches( regmatch_t* const matches, char const* const string, string_buffer_t* const replacement_buffer ) {
	char* replacement = malloc( sizeof(char) * (replacement_buffer-> eos + 1) );
	char* replacement_ptr = replacement;
	size_t i = 0;

	strcpy( replacement, replacement_buffer-> char_at );

	for ( ; i < replacement_buffer-> eos; i++, replacement++ ) {

		if ( replacement_buffer-> char_at[i] == '\\' 
		&& replacement_buffer-> char_at[i+1] >= '1' 
		&& replacement_buffer-> char_at[i+1] <= '9' ) {

			short const match_num = replacement_buffer-> char_at[i+1] - 48;		
			size_t const subexpr_match_len = matches[ match_num ].rm_eo - matches[ match_num ].rm_so;
			
			sbuffer_set_end_of_string( replacement_buffer, i );

			sbuffer_append( 
				replacement_buffer,
				string + matches[ match_num ].rm_so,
				subexpr_match_len
			);

			replacement += REVERSE_LINK_LEN;

			sbuffer_append(
				replacement_buffer,
				replacement,
				strlen(replacement)
			);

			/* beacase on the next iteration, these values will increment. Need refactoring */
			i += subexpr_match_len - 1;
			replacement--;
		}
	}

	free( replacement_ptr );

	return 0;
}

int sub( string_buffer_t* const string_buffer, regex_t const regexp, string_buffer_t* const replacement_buffer, int const flags ) {
	regmatch_t matches[10];
	int const result = regexec( &regexp, string_buffer-> char_at, 10, matches, 0 );
	size_t last_part_length = 0;
	char* last_part;

	if ( result ) return NO_MATCHES;

	last_part_length = string_buffer-> eos - matches[0].rm_eo;
	last_part = malloc( (last_part_length + 1) * sizeof(char) );

	strcpy( last_part, string_buffer-> char_at + matches[0].rm_eo );

	insert_subexpr_matches( 
		matches, 
		string_buffer-> char_at,
		replacement_buffer 
	);

	sbuffer_set_end_of_string(
		string_buffer,
		matches[0].rm_so
	);

	sbuffer_append_buf( 
		string_buffer, 
		replacement_buffer 
	);

	sbuffer_append(
		string_buffer,
		last_part,
		last_part_length
	);

	free( last_part );

	return MATCH;
}

int gsub( string_buffer_t* const string_buffer, regex_t const regexp, string_buffer_t* const replacement_buffer, int const flags ) {
	char* const string = malloc( (string_buffer-> eos + 1) * sizeof(char) );
	string_buffer_t* original_replacement = sbuffer_clone( replacement_buffer );
	size_t i = 0, j = 0;
	size_t const str_len = string_buffer-> eos;

	strcpy( string, string_buffer-> char_at );

	for ( ; ; ) {
		regmatch_t matches[10];	
		int result = regexec( &regexp, string_buffer-> char_at + i, 10, matches, 0 );

		if ( result ) {
			free( string );
			return result;
		}

		insert_subexpr_matches( 
			matches, 
			string_buffer-> char_at + i,
			replacement_buffer
		);

		sbuffer_set_end_of_string( string_buffer, i + matches[0].rm_so );

		sbuffer_append_buf(
			string_buffer, replacement_buffer
		);

		sbuffer_append(
			string_buffer,
			string + j + matches[0].rm_eo,
			str_len - j - matches[0].rm_eo
		);

		i += matches[0].rm_so + replacement_buffer-> eos;
		j += matches[0].rm_eo;

		sbuffer_reinit( replacement_buffer, original_replacement-> char_at );
	}
}

int transform( char* const string, char const* const to_transform_seq, char const* const transform_seq) {
	size_t i = 0;
	size_t string_len = strlen( string );
	size_t char_seq_len = strlen( to_transform_seq );

	for ( ; i < string_len; i++ ) {
		size_t j = 0;
		for ( ; j < char_seq_len; j++ ) {
			if ( string[i] == to_transform_seq[j] ) {
				string[i] = transform_seq[j];
			}
		}
	}

	return 0;
}

/*int main( int argc, char** argv ) {
	char buf[100];
	size_t count = read(STDIN_FILENO, &buf, 99);
	string_buffer_t* string;
	string_buffer_t* replacement;

	regex_t regexp;

	buf[count - 1] = '\0';

	regcomp( &regexp, "\\(a\\)b\\(a\\)", 0 );

	string = sbuffer_init( buf );
	replacement = sbuffer_init( "\\1\\2" );

	gsub( string, regexp, replacement, 0 );
	puts( string-> char_at);

	return 0;
}*/
