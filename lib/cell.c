#include "cassl1.h"

#define THREAD_NUM 8
#define MIN_BUCKET_SIZE 20

static int translate_subarray(state_num_t *cell_array, size_t size, state_num_t left_end, state_num_t right_end);

int translate_array(state_num_t *cell_array, size_t size)
{
  int i;
  int ret;
  int active_thread_num;
  int sub_array_size;
  state_num_t *start_pos;
  state_num_t tmp, save;

  if (size < 3) {
    return ERR_TOO_SMALL;
  }

  if  ((size - 2) / MIN_BUCKET_SIZE + 1 > THREAD_NUM) {
    active_thread_num = THREAD_NUM;
  } else {
    active_thread_num = (size - 2) / MIN_BUCKET_SIZE + 1;
  }

  if (active_thread_num == 1) {
    ret = translate_subarray(cell_array + 1, size - 2 , cell_array[0], cell_array[size + 1]);
    return ret;
  }

  start_pos = cell_array + 1;
  save = *(start_pos - 1);
  for (i = 0; i < active_thread_num; i++) {

    sub_array_size = (size - 2) / active_thread_num;
    if (i == active_thread_num - 1) {
      sub_array_size += (size - 2) % active_thread_num;
    }
    tmp = save;
    save = *(start_pos + sub_array_size - 1);
    ret = translate_subarray(start_pos, sub_array_size, tmp, *(start_pos + sub_array_size));

    start_pos += sub_array_size;
  }

  return ret;
}

static int translate_subarray(state_num_t *cell_array, size_t size, state_num_t left_end, state_num_t right_end)
{
  state_num_t save;
  state_num_t next;

  int i;
  int count_undefined = 0;
  int count_firing = 0;

  for (i = 0, save = left_end; i < size; i++) {
    next = get_next_state(save, cell_array[i],
			  i == size - 1 ? right_end : cell_array[i + 1]);

    if (next == FIRING) {
      count_firing++;
    } else if (next == EXTERNAL) {
      count_undefined++;
    }

    save = cell_array[i];
    cell_array[i] = next;
  }

  if (count_undefined != 0) {
    return ERR_UNDEFINED_RULE;
  }

  if (count_firing != 0) {
    if (count_firing == size) {
      return SYNCHRONIZE;
    } else {
      return ERR_NOT_SYNCHRONIZE;
    }
  }
  return SUCCESS;
}
