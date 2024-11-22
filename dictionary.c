
#include "dictionary.h"
#include "sef_debug.h"
#include "string.h"
#include "stdio.h"

// Static functions
static sef_error double_size(forth_dictionary_t* fd);
static void sort_dic(forth_dictionary_t* fd);
static void free_word(entry_t e);

// This functions double the size of the dynamic array.
static sef_error double_size(forth_dictionary_t* fd) {
    entry_t* new = malloc(sizeof(entry_t) * fd->max * 2);
    if (new == NULL) {
        return sef_no_memory;
    }
    for (size_t i = 0; i < fd->n_entries; i++) {
        new[i] = fd->entries[i];
    }
    free(fd->entries);
    fd->entries = new;
    fd->max = fd->max * 2;
    return sef_OK;
}

// This functions assumes that all the element of the array but the last are sorted and sort the last one.
static void sort_dic(forth_dictionary_t* fd) {
    for (size_t i = fd->n_entries - 1; i > 0; i--) {
        if (fd->entries[i].hash < fd->entries[i - 1].hash) {
            entry_t tmp = fd->entries[i];
            fd->entries[i] = fd->entries[i - 1];
            fd->entries[i - 1] = tmp;
        } else {
            return;
        }
    }
}

// This function frees the memory used in a word
static void free_word(entry_t e) {
#if SEF_STORE_NAME
    free(e.name);
#endif
    switch (e.type) {
        case FORTH_word:   // Part of those words are dynamically allocated
            sef_clean_user_word(e.func.F_word);
            break;
        case string:
            free(e.func.string.data);
            break;
        case compile_word:
            free(e.func.compile_func.payload);
            break;
        case constant:
        case defered:
        case C_word:
        case alias:
            break;
    }
}

// Global API

// This functions initializes a dictionary of the minimum size.
forth_dictionary_t* sef_init_dic(void) {
    forth_dictionary_t* ret = malloc(sizeof(forth_dictionary_t));
    ret->entries = malloc(sizeof(entry_t));
    ret->max = 1;
    ret->n_entries = 0;
#if SEF_CASE_INSENSITIVE == 0
    ret->case_insensitive = true;
#endif
    return ret;
}

// This function frees the memory used by a dictionary
void sef_clean_dic(forth_dictionary_t* fd) {
    for (size_t i = 0; i < fd->n_entries; i++) {
        entry_t e = fd->entries[i];
        free_word(e);
    }
    free(fd->entries);
    free(fd);
}

// Display nicely the content of a dictionary
void sef_display_dictionary(forth_dictionary_t* dic) {
    for (size_t i=0; i<dic->n_entries; i++) {
        char buff[200];
#if SEF_STORE_NAME
        snprintf(buff, 199, "Entry %zu/%zu %s:\n", i+1, dic->n_entries, dic->entries[i].name);
#else
        snprintf(buff, 199, "Entry %zu/%zu:\n", i+1, dic->n_entries);
#endif
        sef_print_string(buff);
        const char* type;
        switch (dic->entries[i].type) {
            case C_word:
                type = "C word";
                break;
            case alias:
                type = "alias";
                break;
            case FORTH_word:
                type = "Forth word";
                break;
            case compile_word:
                type = "compile word";
                break;
            case constant:
                type = "constant";
                break;
            case string:
                type = "string";
                break;
            case defered:
                type = "defered word";
                break;
            default:
                type = "!! INVALID TYPE !!";
                break;
        }
        sprintf(buff, "type = %s, hash = %X\n\n", type, dic->entries[i].hash);
        sef_print_string(buff);
    }
}

// If there is an element in the dictionary with the desired hash,
// put it in e, put its index in index and return sef_OK.
// Otherwise, returns sef_not_found;
// If e or index are NULL, the values are not copied.
sef_error sef_find(forth_dictionary_t* fd, entry_t* e, size_t* index, hash_t hash) {
    if (fd->n_entries == 0) {
        return sef_not_found;
    }
    size_t lower_b = 0;
    size_t upper_b = fd->n_entries;
    size_t target = (lower_b + upper_b) / 2;
    while (fd->entries[target].hash != hash) {
        if (fd->entries[target].hash < hash) {
            if (target == fd->n_entries - 1 || target == fd->n_entries) {
                return sef_not_found;
            }
            if (fd->entries[target + 1].hash > hash) {
                return sef_not_found;
            }
            lower_b = target;
            target = (lower_b + upper_b) / 2;
        } else {
            if (target == 0) {
                return sef_not_found;
            }
            upper_b = target;
            target = (lower_b + upper_b) / 2;
        }
    }
    if (e != NULL) {
        *e = fd->entries[target];
    }
    if (index != NULL) {
        *index = target;
    }
    return sef_OK;
}

// hash_t have their MSB at 0 when they are regular hashes and at 1 when they
// are special hashes (id to index a special content). Thin function returns an
// unused special hash
hash_t sef_unused_special_hash(forth_dictionary_t* fd) {
    const hash_t first_special_hash = SEF_RAW_HASH_MASK + 1;
    if (fd->n_entries == 0) {
        return first_special_hash;
    }
    hash_t last_hash = fd->entries[fd->n_entries-1].hash;
    // The special hashes are at the end of the dictionary. Thus, the last
    // element in the dictionary have the highest special hash. The hash right
    // after it have to be free
    if (last_hash >= first_special_hash) {
        return last_hash + 1;
    } else {
        return first_special_hash;
    }
}

// Register a string in the dictionary under a special hash and return that
// hash
hash_t sef_register_string(forth_dictionary_t* fd, const char* str, size_t size) {
    entry_t e = {
        .type = string,
        .hash = sef_unused_special_hash(fd),
        .func.string.size = size,
        .func.string.data = malloc(size+1),
    };
#if SEF_STORE_NAME
    e.name = malloc(size+3);
    memcpy(e.name+1, str, size);
    e.name[0] = '"';
    e.name[size+1] = '"';
    e.name[size+2] = 0;
#endif
    memcpy(e.func.string.data, str, size);
    e.func.string.data[size] = 0; // Null terminating
    sef_add_elem(fd, e, "");
    return e.hash;
}

// This function adds a new element to the dictionary.
// The size is extended if needed and the dictionary is left sorted
// If an element in the array got a similar hash, it is overwritten
sef_error sef_add_elem(forth_dictionary_t* fd, entry_t e, const char* name) {
    size_t index;
    entry_t old_entry;
    if (sef_find(fd, &old_entry, &index, e.hash) == sef_not_found) {  // We need to add a new element
        if (fd->n_entries == fd->max) {
            sef_error rc = double_size(fd);
            if (rc != sef_OK) {
                return rc;
            }
        }
        fd->entries[fd->n_entries] = e;
        fd->n_entries++;
        sort_dic(fd);
    } else {    // We need to overwrite an element
        if (old_entry.type != defered) { // Devered words should be replaceble with no warnings
#if SEF_STORE_NAME
                warn_msg("Overwriting the word with hash %"PRIx32" named %s.\n", e.hash, old_entry.name);
                if (strcmp(old_entry.name, e.name)) {
                    error_msg("New entry named %s have the same name as old entry named %s. Hash algorithm should be changed.\n", old_entry.name, e.name);
                }
#else
                warn_msg("Overwriting the word with hash %"PRIx32".\n", e.hash);
#endif
        }
        entry_t old = fd->entries[index];
        free_word(old);
        fd->entries[index] = e;
    }
#if SEF_CASE_INSENSITIVE == 0   // Register upper case version of the name as well.
    if (fd->case_insensitive) {
        char name_upper[strlen(name) + 1];
        for (size_t j = 0; j <= strlen(name); j++) {
            if ('a' <= name[j] && name[j] <= 'z') {
                name_upper[j] = name[j] - ('a' - 'A');
            } else {
                name_upper[j] = name[j];
            }
        }
        if (strcmp(name_upper, name)) {
            return sef_set_alias(fd, sef_hash(name_upper), e.hash, name_upper);
        }
    }
#else
    (void) name;
#endif
    return sef_OK;
}

// This function sets the first hash to be an alias to the second.
sef_error sef_set_alias(forth_dictionary_t* fd, hash_t word_hash, hash_t alias_to, const char* name) {
    entry_t new_entry;
    new_entry.hash = word_hash;
    new_entry.type = alias;
    new_entry.func.alias_to = alias_to;
#if SEF_STORE_NAME
    new_entry.name = malloc(strlen(name) + 1);
    strcpy(new_entry.name, name);
#endif
    return sef_add_elem(fd, new_entry, name);
}


// This function calls a known function from the dictionary, the effect will
// vary depending on the type of the function. The function is id by its hash
// If something is not found, print an error message
sef_error sef_call_func(forth_state_t* fs, hash_t hash) {
    entry_t e;
    sef_error find_rc = sef_find(fs->dic, &e, NULL, hash);
    if (find_rc != sef_OK) {
        error_msg("Unable to find desired function with hash %" SEF_HASH_PRINT ".\n", hash);
        return find_rc;
    }
#if SEF_STORE_NAME
    debug_msg("Calling hash %" PRIdPTR " named %s\n", hash, e.name);
#endif
    switch (e.type) {
        case C_word:
            e.func.C_func(fs);
            break;
        case FORTH_word: {
            code_pointer_t old_pos = fs->pos;
            fs->pos.current_word = hash;
            fs->pos.pos_in_word = 0;
            fs->current_word_copy = e.func.F_word;
            sef_push_code(fs, sef_code_pointer_to_int(&old_pos));
            }
            break;
        case alias:
            return sef_call_func(fs, e.func.alias_to);
        case constant:
            sef_push_data(fs, e.func.constant);
            break;
        case defered:
            error_msg("Calling word %s%s%swhich is defered but not defined.\n",
#if SEF_STORE_NAME
                    "\"", e.name, "\" "
#else
                    "", "", ""
#endif
                    );
            return sef_not_found;
        case compile_word:
            error_msg("compile_word not used yet.\n");
            return sef_impossible_error;
        case string:
            sef_push_data(fs, (sef_int_t) e.func.string.data);
            sef_push_data(fs, (sef_int_t) e.func.string.size);
            debug_msg("Pushing string `%s` of size %zu\n", e.func.string.data, e.func.string.size);
            break;
    }
    return sef_OK;
}

// Try to call a function by its name
sef_error sef_call_name(forth_state_t* fs, const char* name) {
    hash_t hash = sef_hash(name);
    sef_error rc = sef_call_func(fs, hash);
    if (rc != sef_OK) {
        error_msg("Can't call function %s.\n", name);
    }
    return rc;
}

