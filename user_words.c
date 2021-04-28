#include "user_words.h"

//Tis function compiles a new user words in the given dictionary
//subword_n is the number of words in our definition
//subwords is the list of the subwords
error amf_compile_user_word(forth_dictionary_t* fd, const char* name, size_t subword_n, const char** subwords) {
	user_word_t* def = malloc(sizeof(user_word_t));
	def->size = subword_n;
	def->content = malloc(sizeof(word_node_t) * subword_n);
	for(size_t i=0; i<subword_n; i++){
		if(0){
			//TODO Check for special words
		}else{
			printf("Registering word %s at pos %i with hash %" WORD_PRINT ".\n", subwords[i], i, amf_hash(subwords[i]));
			def->content[i].type = normal_word;
			def->content[i].content.hash = amf_hash(subwords[i]);
		}
	}
	entry_t e;
	e.hash = amf_hash(name);
	e.type = FORTH_word;
	e.func.F_word = def;
	return amf_add_elem(fd, e);
}

