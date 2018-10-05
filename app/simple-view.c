#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "application.h"


static int print_array(const state_num_t *cell_array, const int cell_size);

int start_simple_view(const option *option)
{
  int ret;

  int step;
  unsigned int cell_size;
  state_num_t *cell_array;

  printf("file: %s, states: %d, rules: %ld\n",
	 option->file_property.path,
	 option->file_property.state_num,
	 option->file_property.rule_num);

  cell_size = option->cell_size + 2;

  if (SOLDIER == 0) {
    cell_array = (state_num_t *)calloc(sizeof(state_num_t), cell_size);
  } else {
    int i;
    cell_array = (state_num_t *)malloc(sizeof(state_num_t) * cell_size);
    for (i = 2; i < cell_size - 1; i++) {
      cell_array[i] = SOLDIER;
    }
  }
  if (cell_array == NULL) {
    printf("OUT_OF_MEMORY\n");
    return ERR_OUT_OF_MEMORY;
  }
  cell_array[0] = cell_array[cell_size -1] = EXTERNAL;
  cell_array[1] = GENERAL;

  printf("cells: %d\n", cell_size - 2);
  print_array(cell_array, cell_size);
  for (step = 0; step < cell_size * 3; step++) {
    if (option->interval > 0) {
      usleep(option->interval * 1000);
    }
    ret =change_state(cell_array, cell_size);
    print_array(cell_array, cell_size);
    if (ret != SUCCESS) {
      break;
    }
  }

  free(cell_array);
  return SUCCESS;
}


static int print_array(const state_num_t *cell_array, const int cell_size)
{
  int i;
  state *state;
  for (i = 1; i < cell_size - 1; i++) {
    state = get_state_byindex(cell_array[i]);
    if (state == NULL) {
      return ERR_UNDEFINED_STATE;
    }
    printf("\x1b[38;2;%d;%d;%dm\x1b[48;2;%d;%d;%dm%c \x1b[0m",
    	   state->fg_rgb_color[0], state->fg_rgb_color[1], state->fg_rgb_color[2],
    	   state->bg_rgb_color[0], state->bg_rgb_color[1], state->bg_rgb_color[2],
    	   state->name[0]);
  }
  putchar('\n');
  return SUCCESS;
}
