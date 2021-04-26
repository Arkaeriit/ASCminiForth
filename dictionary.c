
#include "dictionary.h"
#include "string.h"

//Static functions
static void double_size(forth_dictionary_t* fd);
static void sort_dic(forth_dictionary_t* fd);

//This functions double the size of the dinamic array.
static void double_size(forth_dictionary_t* fd){
    entry_t* new = malloc(sizeof(entry_t) * fd->max * 2);
    for(size_t i=0; i<fd->n_entries; i++){
        memcpy(new[i], fd->entries[i], sizeof(entry_t));
    }
    free(fd->entries);
    fd->entries = new;
    fd->max = fd->max * 2;
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

//This functions initializes a dictionary of the minimum size.
forth_dictionary_t* fd_init(void){
    forth_dictionary_t* ret = malloc(sizeof(forth_dictionary_t));
    ret->entries = malloc(sizeof(entry_t));
    ret->max = 1;
    ret->n_entries = 0;
    return 0;
}

//This function frees the memory used by a dictionary
void fd_clean(forth_dictionary_t* fd){
    free(fd->entries);
    free(fd);
}

//This function adds a new element to the dictionary.
//The size is extended if needed and the dictionary is left sorted
void fd_add_elem(entry_t e){
    if(fd->n_entries == fd->max){
        double_size(fd);
    }
    fd->entries[fd->n_entries] = e;
    fd->n_entries++;
    sort_dic(fd);
}



