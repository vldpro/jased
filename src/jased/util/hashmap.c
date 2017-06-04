#include <malloc.h>
#include <string.h>

#include "jased/util/hashmap.h"

typedef struct entry_t {
	int value;
	char* key;
	struct entry_t* next;
} entry_t;

typedef struct chain_t {
	entry_t* front;
	size_t length;
} chain_t;

struct hashmap_t {
	hash_function_t hash;
	chain_t** container;
	size_t size;
};

/*Entries API*/
chain_t* create_chain(void) {
	chain_t* new_chain = (chain_t*)malloc(sizeof(chain_t));

	new_chain-> front = NULL;
	new_chain-> length = 0;

	return new_chain;
}

char* get_copy(char* src) {
	char* dest = (char*)malloc(strlen(src) + 1);
	return strcpy(dest, src);
}

entry_t* create_entry(chain_t* chain, char* const key, int const value) {
	entry_t* new_entry = (entry_t*)malloc(sizeof(entry_t));

	new_entry-> key = get_copy(key);
	new_entry-> value = value;

	new_entry-> next = chain->front;
	chain-> front = new_entry;

	chain->length++;
	return new_entry;
}

static entry_t* chain_get_entry( chain_t* const chain, char* const key ) {
	entry_t* current_entry = chain-> front;
	if(!current_entry) return NULL;

	do {
		if( !strcmp(current_entry->key, key) ) 
			return current_entry;
	} while( (current_entry = current_entry->next) != NULL );

	return NULL;
}

static int chain_get_value(chain_t* const chain, char* const key) {
	entry_t* entry;
	if( (entry = chain_get_entry(chain, key)) )
		return entry->value;
	return -200;
}

static int is_present( chain_t* const chain, char* const key ) {
	return chain_get_entry(chain, key) != NULL;
}

static int chain_add( chain_t* const chain, char* const key, int const value) {
	if( is_present(chain, key) ) 
		return 1;

	create_entry( chain, key, value );

	return 0;
}

static int chain_set(chain_t* const chain, char* const key, int const value) {
	entry_t* old_entry;

	if( (old_entry = chain_get_entry(chain, key)) ) {
		old_entry-> value = value;
    }

	return old_entry != NULL;
}

static void remove_entry( entry_t* to_clear ) {
	free( to_clear->key );
	free( to_clear );
}

static int chain_remove( chain_t* const chain, char* const key) {
	entry_t* current_entry = chain-> front;
	entry_t* previous_entry = NULL;

	if(!current_entry) return 0;

	do {
		if(!strcmp(current_entry->key, key)) 
			break;
		previous_entry = current_entry;

	} while( (current_entry = current_entry->next) != NULL );
	
	if( !current_entry ) return 0;

	if( previous_entry ) {
		previous_entry-> next = current_entry->next;
	} else {
		chain->front = current_entry->next;
	}

	chain->length--;
	remove_entry( current_entry );

	return 1;
}

static void chain_free( chain_t* chain ) {
	entry_t* chain_front = chain->front;
	entry_t* to_clear;

	while( (to_clear = chain_front) != NULL ) {
		chain_front = chain_front->next;	
		remove_entry(to_clear);
	}
}

/*Hashmap API*/
hashmap_t* hmap_new( hash_function_t hash, size_t const size ) {
	hashmap_t* new_hmap = (hashmap_t*)malloc( sizeof(hashmap_t) );

	new_hmap-> container = (chain_t**)calloc( sizeof(chain_t*), size );
	new_hmap-> hash = hash;
	new_hmap-> size = size;

	return new_hmap;
}

static size_t get_index( size_t const hmap_size, size_t const hcode ) {
	return hcode % hmap_size;
}

int hmap_add( hashmap_t* const hmap, char* const key, int const value ) {
	size_t idx = get_index( hmap->size, hmap-> hash(key) );

	if(hmap-> container[idx] == NULL) 
		hmap-> container[idx] = create_chain();

	return chain_add( hmap-> container[idx], key, value );
}

int hmap_get( hashmap_t* const hmap, char* const key ) {
	size_t idx = get_index( hmap->size, hmap->hash(key));
	
	if( !hmap->container[idx] ) return -200;	

	return chain_get_value(hmap->container[idx], key);
}

int hmap_erase( hashmap_t* const hmap, char* const key ) {
	size_t idx = get_index(hmap->size, hmap->hash(key));

	if( !hmap->container[idx] ) return 0;	

	return chain_remove(hmap->container[idx], key);
}

void hmap_delete( hashmap_t* hmap ) {
	size_t i = 0;		
	for( i = 0; i < hmap-> size; i++ ) {
		chain_t* current_chain = hmap-> container[i];
		if( current_chain != NULL ) {
			chain_free(current_chain); 
			hmap->container[i] = NULL;
		}
	}

    free(hmap-> container);
    free(hmap);
}

int hmap_set( hashmap_t* const hmap, char* const key, int const value ) {
	size_t idx = get_index( hmap->size, hmap->hash(key) );

	if( !hmap-> container[idx] ) return 0;
		
	return chain_set(hmap->container[idx], key, value);
}

int default_hash( char* value ) {
	int hcode = 1;
	
	while(*value){
		hcode += *value;
		hcode >>= 4;
		value++;
	}

	return hcode;
}

void hmap_debug( hashmap_t* const hmap ) {
	size_t size = hmap->size;
	size_t i = 0;

	for(i = 0; i < size; i++) {
		chain_t* current_chain = hmap->container[i];	
		if(current_chain != NULL)
			printf("Load coef hash[%d]: %d\n", (int)i, (int)current_chain->length);
	}
}
