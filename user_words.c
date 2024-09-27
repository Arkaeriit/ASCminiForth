#include "user_words.h"
#include "amf_debug.h"
#include "string.h"

static int words_in_str(const char* str);
static char** cut_string(const char* str, size_t* list_size);
static bool str_to_num(const char* str, amf_int_t* num, int base);

// This function compiles a new user words in the given dictionary
// subword_n is the number of words in our definition
// subwords is the list of the subwords
error amf_compile_user_word(forth_dictionary_t* fd, const char* name, size_t subword_n, char** subwords, int base) {
    user_amf_int_t* def = malloc(sizeof(user_amf_int_t));
    def->size = subword_n;
    if (def->size >= (1 << AMF_WORD_CONTENT_SIZE_BITS)) {
        free(def);
        error_msg("Error, trying to register a word with %zi (%s) subwords while the maximum is %zi.\n", subword_n, name, (1 << AMF_WORD_CONTENT_SIZE_BITS) - 1);
        return amf_config_error;
    }
    def->content = malloc(sizeof(word_node_t) * subword_n);
    for (size_t i = 0; i < subword_n; i++) {
        def->content[i] = amf_compile_node(subwords[i], base);
#if AMF_LOG > 1
        if (def->content[i].type == normal_word) {
            error find_rc = amf_find(fd, NULL, NULL, amf_hash(subwords[i]));
            if (find_rc != OK) {
                warn_msg("The word %s is needed but it is not in the dictionary.\n", subwords[i]);
            }
        }
#endif
    }
    entry_t e;
    e.hash = amf_hash(name);
    e.type = FORTH_word;
    e.func.F_word = def;
#if AMF_STORE_NAME
    e.name = malloc(strlen(name) + 1);
    strcpy(e.name, name);
#endif
    return amf_add_elem(fd, e, name);
}

word_node_t amf_compile_node(const char* str, int base) {
    word_node_t ret;
    // Checking if the string is a number
    amf_int_t num;
    if (str_to_num(str, &num, base)) {
        ret.type = raw_number;
        ret.content.value = num;
        // Checking if it is a raw string
    } else {
        debug_msg("Registering word %s with hash %" AMF_INT_PRINT ".\n", str, amf_hash(str));
        ret.type = normal_word;
        ret.content.hash = amf_hash(str);
    }
    return ret;
}

static const char* word_delimiters = " \t\n\r";
// This functions takes a string such as "1 1 + ." and compiles it
// as a C word
error amf_compile_string(forth_dictionary_t* fd, const char* name, const char* str, int base) {
    size_t nwords;
    char** subwords = cut_string(str, &nwords);
    debug_msg("There is %i words in the definition of %s [%s].\n", nwords, name, str);
    error rc = amf_compile_user_word(fd, name, nwords, subwords, base);
    for (size_t i = 0; i < nwords; i++) {
        free(subwords[i]);
    }
    free(subwords);
    return rc;
}

// Count the number of words in a string
static int words_in_str(const char* str) {
    int ret = 0;
    bool pointing_to_word = false;
    bool in_quotes = false;
    for (size_t i = 0; i < strlen(str); i++) {
        if (str[i] == '"') {
            in_quotes = !in_quotes;
        }
        if (in_quotes) {
            continue;
        }
        if (pointing_to_word) {
            if (amf_is_delimiter(str[i])) {
                pointing_to_word = false;
            }
        } else {
            if (!amf_is_delimiter(str[i])) {
                pointing_to_word = true;
                ret++;
            }
        }
    }
    return ret;
}

// Tell if a char is a member of the list of word delimiters
bool amf_is_delimiter(char ch) {
    for (size_t i = 0; i < strlen(word_delimiters); i++) {
        if (word_delimiters[i] == ch) {
            return true;
        }
    }
    return false;
}

// From a string, return a list of each words.
// The second argument will contain the number of elements i the list
static char** cut_string(const char* str, size_t* list_size) {
    int n_words = words_in_str(str);
    char** ret = malloc(sizeof(char *) * n_words);
    size_t word_included = 0;
    bool pointing_to_word = false;
    size_t word_start;
    bool in_quotes = false;
    for (size_t i = 0; i < strlen(str); i++) {
        if (str[i] == '"') {
            in_quotes = !in_quotes;
        }
        if (in_quotes) {
            continue;
        }
        if (pointing_to_word) {
            if (amf_is_delimiter(str[i])) {
                pointing_to_word = false;
                ret[word_included] = malloc(i - word_start + 1);
                memcpy(ret[word_included], str + word_start, i - word_start);
                ret[word_included][i - word_start] = 0;
                word_included++;
            }
        } else {
            if (!amf_is_delimiter(str[i])) {
                pointing_to_word = true;
                word_start = i;
            }
        }
    }
    if (pointing_to_word) {
        size_t i = strlen(str);
        ret[word_included] = malloc(i - word_start + 1);
        memcpy(ret[word_included], str + word_start, i - word_start);
        ret[word_included][i - word_start] = 0;
        word_included++;
    }
    *list_size = word_included;
    return ret;
}

// Convert a string to a number, return true if the string was a number and
// false otherwise. stroll cannot be used alone as we need to differentiate
// valid 0 from errors.
static bool str_to_num(const char* str, amf_int_t* num, int base) {
    char* end;
    *num = strtoll(str, &end, base);
    if ((size_t) (end - str) == strlen(str)) {
        return true;
    }
    if ((size_t) (end - str) == strlen(str) - 1 && str[strlen(str)-1] == '.' && strlen(str) != 1) {
        return true;
    }
    return false;
}

// This functions frees the memory used in an user word
void amf_clean_user_word(user_amf_int_t* w) {
    for (size_t i = 0; i < w->size; i++) {
        word_node_t n = w->content[i];
        switch (n.type) {
            case normal_word:
            case raw_number:
                break;
        }
    }
    free(w->content);
    free(w);
}

// This function registers a new constant
error amf_compile_constant(const char* name, forth_state_t* fs) {
    entry_t e;
    e.hash = amf_hash(name);
    e.type = constant;
    e.func.constant = amf_pop_data(fs);
#if AMF_STORE_NAME
    e.name = malloc(strlen(name) + 1);
    strcpy(e.name, name);
#endif
    return amf_add_elem(fs->dic, e, name);
}

