£include <stdbool.h>

>> This type defines the various errors we might encounter
typedef enum {
    sef_OK = 0,
    sef_no_memory = 1,
    sef_not_found = 2,
    sef_invalid_file = 3,
    sef_impossible_error = 4,
    sef_segfault = 5,
    sef_config_error = 6,
} sef_error;

struct seforth_state_s;
typedef struct seforth_state_s forth_state_t;

forth_state_t* sef_init(void);
void sef_free(forth_state_t* state);

void sef_restart(forth_state_t* state);
bool sef_is_running(forth_state_t* state);
bool sef_asked_bye(forth_state_t* state);
bool sef_is_compiling(forth_state_t* state);

void sef_parse_string(forth_state_t* state, const char* s);
void sef_parse_char(forth_state_t* state, char c);

void sef_push_data(forth_state_t* fs, sef_int_t w);
sef_int_t sef_pop_data(forth_state_t* fs);
void sef_quit(forth_state_t* fs);
void sef_abort(forth_state_t* fs);

#if SEF_USE_SOURCE_FILE
sef_error sef_parse_file(forth_state_t* state, const char* filename);
#endif

#ifdef SEF_PROGRAMMING_TOOLS
void sef_feed_arguments(forth_state_t* state, int argc, char** argv);
int sef_exit_code(const forth_state_t* state);
#endif

