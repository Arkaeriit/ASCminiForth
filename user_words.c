#include "user_words.h"
#include "string.h"

static int words_in_str(char* str);

//This function compiles a new user words in the given dictionary
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
			debug_msg("Registering word %s at pos %i with hash %" WORD_PRINT ".\n", subwords[i], i, amf_hash(subwords[i]));
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

static const char* word_delimiters = " \t\n\r";
//This functions takes a string such as "1 1 + ." and compiles it
//as a C word
error amf_compile_string(forth_dictionary_t* fs, const char* name, char* str){
   int nwords = words_in_str(str); 
   debug_msg("There is %i words in the definition of %s.\n", nwords, name);
   return OK;
}

static int words_in_str(char* str){
    error_msg("1\n");
    const char* pnt = strtok(str, word_delimiters);
    error_msg("2\n");
    int ret = 0;
    while (pnt != NULL){
        strtok(NULL, word_delimiters);
        ret++;
    }
    return ret;
}

