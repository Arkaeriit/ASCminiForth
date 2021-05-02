#include "user_words.h"
#include "string.h"

static int words_in_str(const char* str);
static char** cut_string(const char* str);
static bool str_to_num(const char* str, word_t* num);

//This function compiles a new user words in the given dictionary
//subword_n is the number of words in our definition
//subwords is the list of the subwords
error amf_compile_user_word(forth_dictionary_t* fd, const char* name, size_t subword_n, const char** subwords) {
	user_word_t* def = malloc(sizeof(user_word_t));
	def->size = subword_n;
	def->content = malloc(sizeof(word_node_t) * subword_n);
	for(size_t i=0; i<subword_n; i++){
        def->content[i] = amf_compile_node(subwords[i]); 
	}
	entry_t e;
	e.hash = amf_hash(name);
	e.type = FORTH_word;
	e.func.F_word = def;
	return amf_add_elem(fd, e);
}

word_node_t amf_compile_node(const char* str){
    word_node_t ret;
    //Checking if the string is a number
    word_t num;
    if(str_to_num(str, &num)){
        ret.type = raw_number;
        ret.content.value = num;
    //Else, its a call to an other word
    }else{
        debug_msg("Registering word %s with hash %" WORD_PRINT ".\n", str, amf_hash(str));
        ret.type = normal_word;
        ret.content.hash = amf_hash(str);
    }
    return ret;
}

static const char* word_delimiters = " \t\n\r";
//This functions takes a string such as "1 1 + ." and compiles it
//as a C word
error amf_compile_string(forth_dictionary_t* fd, const char* name, const char* str){
   int nwords = words_in_str(str); 
   debug_msg("There is %i words in the definition of %s [%s].\n", nwords, name, str);
   char** subwords = cut_string(str);
   error rc = amf_compile_user_word(fd, name, nwords, subwords);
   for(int i=0; i<nwords; i++){
       free(subwords[i]);
   }
   free(subwords);
   return rc;
}

//Count the number of words in a string
static int words_in_str(const char* str){
    int ret= 0;
    bool pointing_to_word = false;
    for(size_t i=0; i<strlen(str); i++){
       if(pointing_to_word){
          if(amf_is_delimiter(str[i])){
             pointing_to_word = false;
          }
       }else{
          if(!amf_is_delimiter(str[i])){
             pointing_to_word = true;
            ret++;
          }
       } 
    }
    return ret;
}

//Tell if a char is a member of the list of word delimiters
bool amf_is_delimiter(char ch){
    for(size_t i=0; i<strlen(word_delimiters); i++){
        if(word_delimiters[i] == ch){
            return true;
        }
    }
    return false;
}

//From a string, return a list of each words.
static char** cut_string(const char* str){
    int n_words = words_in_str(str);
    char** ret = malloc(sizeof(char*) * n_words);
    int word_included = 0;
    bool pointing_to_word = false;
    size_t word_start;
    for(size_t i=0; i<strlen(str); i++){
        if(pointing_to_word){
            if(amf_is_delimiter(str[i])){
                pointing_to_word = false;
                ret[word_included] = malloc(i - word_start + 1);
                memcpy(ret[word_included], str + word_start, i - word_start);
                ret[word_included][i - word_start] = 0;
                word_included++;
            }
        }else{
            if(!amf_is_delimiter(str[i])){
                pointing_to_word = true;
                word_start = i;
            }
        } 
    }
    if(pointing_to_word){
        size_t i = strlen(str);
        ret[word_included] = malloc(i - word_start + 1);
        memcpy(ret[word_included], str + word_start, i - word_start);
        ret[word_included][i - word_start] = 0;
    }
    return ret;
}

//Convert a string to a number, return true if the string was a number
//and false otherwize. Should be more minimal than scanf
static bool str_to_num(const char* str, word_t* num){
    *num = 0;
    for(int i=0; i<strlen(str); i++){ //Strating by the MSD
        *num *= 10; //The previous digit had more value than the current one so we multyply it
        if('0' <= str[i] && str[i] <= '9'){
            *num += str[i] - '0';
        }else{
            return false;
        }
    }
    return true;
}

