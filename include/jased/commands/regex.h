/**
* Basic regex operations for jased stream editor
* \author vldpro
* \version 0.01
* \warning not tested
*/
#ifndef JASED_REGEX
#define JASED_REGEX
#include <regex.h>
#include <jased/util/string_buffer.h>

#define NO_MATCHES 1
#define MATCH 0


/** 	
*
* \brief exec regular expression with string
* \param[in] string - string for searching substring, which match with regex
* \param[in] regexp - compiled regular expression
*
* \return 0 - if least at one substring matched with regex, another <>0;
*/	
int match ( char const* const string, regex_t const regexp );


/* 
* \brief subtitute first substring, which matched with regex
* \param[out] string_buffer - contains string agter subtitution
* \param[in] regexp - compiled regular expression
* \param[in] replacement_buffer - string which replace matched substrings in string_buffer
* \param[in] flags - substitution flags
*
* \return 0 - if least at one substring matched with regex, another <>0;
*/
int sub( string_buffer_t* const string_buffer, regex_t const regexp, string_buffer_t* const replacement_buffer, int const flags );


/* 
* \brief subtitute all substring, which matched with regex
* \param[out] string_buffer - contains string agter subtitution
* \param[in] regexp - compiled regular expression
* \param[in] replacement_buffer - string which replace matched substrings in string_buffer
* \param[in] flags - substitution flags
*
* \return 0 - if least at one substring matched with regex, another <>0;
*/
int gsub( string_buffer_t* const string_buffer, regex_t const regexp, string_buffer_t* const replacement_buffer, int const flags );



/**
*	\brief if any characters in string equals with character of  to_transform_seq, they are replaced by approprite character from trasform_seq
*   \param[out] string - string with replaced characters
*	\param[in] to_transform_seq - sequence which contains characters for replacing
*  	\param[in] trasform_seq - 
*
*	\return 0 - if string have at least one character from to_transform_string, another <>0
*/
int transform( char* const string, char const* const to_transform_seq, char const* const transform_seq);

#endif
