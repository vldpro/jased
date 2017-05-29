#ifndef JASED_HASHMAP
#define JASED_HASHMAP

#define HMAP_ELEMENT_IS_PRESENT 1
#define HMAP_SUCCESS 0

typedef struct hashmap_t hashmap_t;
typedef int (*hash_function_t) (char*);

hashmap_t* hmap_new( hash_function_t hash, size_t const size );

void hmap_delete( hashmap_t* hmap );

int hmap_add( hashmap_t* const hmap, char* const key, int const value );

int hmap_set( hashmap_t* const hmap, char* const key, int const value );

int hmap_get( hashmap_t* const hmap, char* const key );

int hmap_erase( hashmap_t* const hmap, char* const key );

void hmap_debug( hashmap_t* const hmap );


int default_hash( char* string );

#endif
