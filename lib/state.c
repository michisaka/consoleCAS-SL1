#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "cassl1.h"

static state_num_t state_number = 0;
static state *states = NULL;
static state *sorted_states = NULL;

static state_num_t special_states_count = 0;
static state_num_t other_states_count = 0;

static int state_cmp(const void *a, const void *b);
static int state_name_cmp(const void *a, const void *b);

int allocate_states(size_t state_number_arg)
{
  if (states != NULL || sorted_states != NULL) {
    return ERR_ALREADY_ALLOCATED;
  }

  if (state_number_arg < 4) {
    return ERR_TOO_SMALL;
  }

  /* TODO: TOO_BIG を検査する */

  states = (state *)calloc(sizeof(state), state_number_arg);
  sorted_states = (state *)calloc(sizeof(state), state_number_arg);

  if (sorted_states == NULL || states == NULL) {
    free_states();
    return ERR_OUT_OF_MEMORY;
  }

  state_number = state_number_arg;
  return SUCCESS;
}

int register_state(state state_arg)
{
  assert(states != NULL);
  assert(sorted_states != NULL);

  if (state_arg.type != OTHERS && states[state_arg.type].name[0] != '\0') {
    return ERR_ALREADY_REGISTERED_TYPE;
  }

  if (get_state_byname(state_arg.name) != NULL) {
    return ERR_ALREADY_REGISTERED_NAME;
  }

  if (state_arg.type == OTHERS) {
    if (other_states_count >= state_number - 4) {
      return ERR_TOO_MANY;
    }
    state_arg.index = 4 + other_states_count;
    states[4 + other_states_count] = state_arg;
    sorted_states[special_states_count + other_states_count] = state_arg;
    other_states_count++;
  } else {
    state_arg.index = state_arg.type;
    states[state_arg.type] = state_arg;
    sorted_states[special_states_count + other_states_count] = state_arg;
    special_states_count++;
  }

  qsort(sorted_states, special_states_count + other_states_count, sizeof(state), state_cmp);

  return SUCCESS;
}

int has_all_special_state(void)
{
  return special_states_count == 4;
}

state* get_state_byname(const char *name)
{
  assert(sorted_states != NULL);

  return bsearch(name, sorted_states, special_states_count + other_states_count, sizeof(state), state_name_cmp);
}

state* get_state_byindex(state_num_t index)
{
  assert(states != NULL);

  if (states[index].name[0] == '\0') {
    return NULL;
  }

  return &states[index];
}

void free_states(void)
{
  if (states != NULL) {
    free(states);
    states = NULL;
  }

  if (sorted_states != NULL) {
    free(sorted_states);
    sorted_states = NULL;
  }

  state_number = 0;
  special_states_count = 0;
  other_states_count = 0;
}

/* for qsort */
static int state_cmp(const void *a, const void *b)
{
  return strcmp(((state *)a)->name, ((state *)b)->name);
}

/* for bsearch */
static int state_name_cmp(const void *a, const void *b)
{
  return strcmp((char *)a, ((state *)b)->name);
}

