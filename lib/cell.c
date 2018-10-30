#include "cassl1.h"

int translate_array(state_num_t *cell_array, size_t size)
{
  state_num_t save;
  state_num_t next;

  int i;
  int count_undefined = 0;
  int count_firing = 0;

  if (size < 3) {
    return ERR_TOO_SMALL;
  }

  for (i = 1, save = cell_array[0]; i < size - 1; i++) {
    next = get_next_state(save, cell_array[i], cell_array[i + 1]);

    if (next == FIRING) {
      count_firing++;
    } else if (next == EXTERNAL) {
	count_undefined++;
    }

    save = cell_array[i];
    cell_array[i] = next;
  }

  if (count_undefined) {
    return ERR_UNDEFINED_RULE;
  }

  if (count_firing) {
    if (count_firing == size - 2) {
      return SYNCHRONIZE;
    } else {
      return ERR_NOT_SYNCHRONIZE;
    }
  }
  return SUCCESS;
}
