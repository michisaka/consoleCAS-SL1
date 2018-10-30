#if !defined(CASSL1_H_INCLUDED)
#define CASSL1_H_INCLUDED

#include <stdio.h>

typedef unsigned char state_num_t;

typedef enum state_type {
		 EXTERNAL = 0,
		 FIRING   = 1,
		 SOLDIER  = 2,
		 GENERAL  = 3,
		 OTHERS   = 4,
} state_type;

typedef struct state {
  char name[6];
  state_type type;
  unsigned char fg_rgb_color[3];
  unsigned char bg_rgb_color[3];
  state_num_t index;
} state;

typedef struct file_property {
  char path[256];
  state_num_t state_num;
  size_t rule_num;
} file_property;

/* return code */
#define SUCCESS     1
#define VALID       2
#define SYNCHRONIZE 3

#define ERR_FILE_IO_ERROR            -1
#define ERR_OUT_OF_MEMORY            -2
#define ERR_ALREADY_ALLOCATED        -3
#define ERR_ALREADY_REGISTERED_TYPE  -4
#define ERR_ALREADY_REGISTERED_NAME  -5
#define ERR_ALREADY_REGISTERED_RULE  -6
#define ERR_TOO_MANY                 -7
#define ERR_TOO_SHORT                -8
#define ERR_TOO_BIG                  -9
#define ERR_TOO_SMALL               -10
#define ERR_UNDEFINED_STATE         -11
#define ERR_INVALID_RULE            -12
#define ERR_UNDEFINED_RULE          -13
#define ERR_NOT_SYNCHRONIZE         -14
#define ERR_UNKNOWN_FORMAT          -15
#define ERR_CURSES_ERROR            -16
#define ERR_THREAD_ERROR            -17

/* liblary capability flags */
#define RULECAP_ALLOCATE_BY_STATENUM 0x1

/* state */
int allocate_states(size_t state_number_arg);
int register_state(state state);
int has_all_special_state(void);
state* get_state_byname(const char *name);
state* get_state_byindex(state_num_t index);
void free_states(void);

/* rule */
extern const unsigned int RULECAP;

int allocate_ruleset(size_t param);
int register_rule(const char *left, const char *center, const char *right, const char *next);
state_num_t get_next_state(state_num_t l_index, state_num_t c_index, state_num_t r_index);
void free_ruleset(void);

/* cell */
int translate_array(state_num_t *cell_array, size_t size);

/* rulefile */
int load_rulefile(const char *path, file_property *property);
#endif /* CASSL1_H_INCLUDED */
