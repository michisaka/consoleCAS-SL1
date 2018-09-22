#include <stdlib.h>
#include <assert.h>
#include <math.h>

#include "cassl1.h"

const unsigned int RULECAP = RULECAP_ALLOCATE_BY_STATENUM;

static state_num_t *ruleset = NULL;
static unsigned int require_bits = 0;

static int check_rule(state_num_t l_index, state_num_t c_index, state_num_t r_index, state_num_t n_index);
static size_t generate_index(state_num_t l_index, state_num_t c_index, state_num_t r_index);

int allocate_ruleset(size_t param)
{
  int length;

  if (ruleset != NULL) {
    return ERR_ALREADY_ALLOCATED;
  }

  if (param < 4) {
    return ERR_TOO_SMALL;
  }

  /* TODO: TOO_BIG を検査する */

  require_bits = (int)ceil(log2(param));
  length = (int)exp2(require_bits * 3);

  if (EXTERNAL == 0) {
    ruleset = (state_num_t *)calloc(sizeof(state_num_t), length);
  } else {
    int i;
    ruleset = (state_num_t *)malloc(sizeof(state_num_t) * length);
    for (i = 0; i < length; i++) {
      ruleset[i] = EXTERNAL;
    }
  }

  if (ruleset == NULL) {
    return ERR_OUT_OF_MEMORY;
  }
  return SUCCESS;
}

int register_rule(const char *left, const char *center, const char *right, const char *next)
{
  state_num_t l_index, c_index, r_index, n_index;
  state *state;
  size_t ruleset_index;
  int check_result;

  assert(ruleset != NULL);

  state = get_state_byname(left);
  if (state == NULL) {
    return ERR_UNDEFINED_STATE;
  }
  l_index = state->index;

  state = get_state_byname(center);
  if (state == NULL) {
    return ERR_UNDEFINED_STATE;
  }
  c_index = state->index;

  state = get_state_byname(right);
  if (state == NULL) {
    return ERR_UNDEFINED_STATE;
  }
  r_index = state->index;

  state = get_state_byname(next);
  if (state == NULL) {
    return ERR_UNDEFINED_STATE;
  }
  n_index = state->index;

  check_result = check_rule(l_index, c_index, r_index, n_index);
  if (check_result != VALID) {
    return check_result;
  }

  ruleset_index = generate_index(l_index, c_index, r_index);

  ruleset[ruleset_index] = n_index;
  return SUCCESS;
}

state_num_t get_next_state(state_num_t l_index, state_num_t c_index, state_num_t r_index)
{
  size_t ruleset_index;

  assert(ruleset != NULL);

  ruleset_index = generate_index(l_index, c_index, r_index);
  return ruleset[ruleset_index];
}

void free_ruleset(void)
{
  if (ruleset != NULL) {
    free(ruleset);
    ruleset = NULL;
  }
}

static int check_rule(state_num_t l_index, state_num_t c_index, state_num_t r_index, state_num_t n_index)
{
  if (c_index == EXTERNAL || n_index == EXTERNAL) {
    return ERR_INVALID_RULE;
  }

  if (l_index == FIRING || c_index == FIRING || r_index == FIRING) {
    return ERR_INVALID_RULE;
  }

  if (get_next_state(l_index, c_index, r_index) != 0) {
    return ERR_ALREADY_REGISTERED_RULE;
  }

  return VALID;
}

static size_t generate_index(state_num_t l_index, state_num_t c_index, state_num_t r_index)
{
  size_t index;
  index = l_index << (require_bits * 2) | c_index << require_bits | r_index;

  return index;
}
