#include <string.h>
#include <stddef.h>

#include "jased/util/string_buffer.h" 
#include "jased/util/chars_queue.h" 

string_buffer_t* escape( string_buffer_t* const src ) {
    chars_queue_t* cq = cqueue_new( sbuffer_new() ); 
    char* src_str = src-> char_at;
    string_buffer_t* dest;

    size_t i = 0;
    while ( src_str[i] != '\0' ) {
        if ( src_str[i] == '\\' ) {
            i++;
            switch( src_str[i] ) {
                case 'n': cqueue_push_back(cq, '\n'); break;
                case 't': cqueue_push_back(cq, '\t'); break;
                case 'r': cqueue_push_back(cq, '\r'); break;
                case 'b': cqueue_push_back(cq, '\b'); break;
                case 'f': cqueue_push_back(cq, '\f'); break;
                case 'v': cqueue_push_back(cq, '\v'); break;

                default: {
                    cqueue_push_back(cq, '\\');
                    cqueue_push_back(cq, src_str[i]);
                }
            }

        } else cqueue_push_back(cq, src_str[i]);

        i++;
    }
    
    dest = sbuffer_truncate(cq-> buffer);
    cqueue_delete(cq);

    return dest;
}
