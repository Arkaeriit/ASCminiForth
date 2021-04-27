
#include "dictionary.h"
#include "string.h"

//Static functions
static error double_size(forth_dictionary_t* fd);
static void sort_dic(forth_dictionary_t* fd);

//This functions double the size of the dinamic array.
static error double_size(forth_dictionary_t* fd){
    entry_t* new = malloc(sizeof(entry_t) * fd->max * 2);
    if(new == NULL){
        return no_memory;
    }
    for(size_t i=0; i<fd->n_entries; i++){
        new[i] = fd->entries[i];
    }
    free(fd->entries);
    fd->entries = new;
    fd->max = fd->max * 2;
    return OK;
}

//This functions assumes that all the elemnt of the aray but the last are sorted and sort the last one.
static void sort_dic(forth_dictionary_t* fd) {
    for(size_t i=fd->n_entries-1; i>0; i--){
        if(fd->entries[i].hash < fd->entries[i-1].hash){
            entry_t tmp = fd->entries[i];
            fd->entries[i] = fd->entries[i-1];
            fd->entries[i-1] = tmp;
        }else{
            return;
        }
    }
}

//Global API

//This functions initializes a dictionary of the minimum size.
forth_dictionary_t* amf_init_dic(void){
    forth_dictionary_t* ret = malloc(sizeof(forth_dictionary_t));
    ret->entries = malloc(sizeof(entry_t));
    ret->max = 1;
    ret->n_entries = 0;
    return 0;
}

//This function frees the memory used by a dictionary
void amf_clean_dic(forth_dictionary_t* fd){
    free(fd->entries);
    free(fd);
}

//If there is an element in the dictionary with the desired hash,
//put it in e, put its index in index and return OK.
//Otherwize, returns not_found;
//If e or index are NULL, the values are not copied.
error amf_find(forth_dictionary_t* fd, entry_t* e, size_t* index, hash_t hash){
    size_t target = fd->n_entries / 2;
    while(fd->entries[target].hash != hash){
        if(fd->entries[target].hash < hash){
            if(target == fd->n_entries){
                return not_found;
            }
            if(fd->entries[target+1].hash > hash){
                return not_found;
            }
            target = (target + fd->n_entries) / 2;
        }else{
            if(target == 0){
                return not_found;
            }
            target = target / 2;
        }
    }
    if(e != NULL){
        *e = fd->entries[target];
    }
    if(index != NULL){
        *index = target;
    }
    return OK;
}

//This function adds a new element to the dictionary.
//The size is extended if needed and the dictionary is left sorted
//If an element in the array got a similar hash, it is overwritten
error amf_add_elem(forth_dictionary_t* fd, entry_t e){
    size_t index;
    if(amf_find(fd, NULL, &index, e.hash) == not_found){ //We need to add a new element
        if(fd->n_entries == fd->max){
            error rc = double_size(fd);
            if(rc != OK){
                return rc;
            }
        }
        fd->entries[fd->n_entries] = e;
        fd->n_entries++;
        sort_dic(fd);
        return OK;
    }else{ //We need to overwrite an element
        entry_t old = fd->entries[index];
        //TODO: add type-specific cleaning code
        fd->entries[index] = e;
        return OK;
    }
}

