#include <curses.h>
#include <stdlib.h>
#include <unistd.h>

#include "cassl1.h"
#include "application.h"
#include "curses-view.h"
#include "version.h"

static state_num_t* allocate_cell_array(size_t cell_size);

int start_curses_view(const option *option)
{
  int ret;
  int step;
  int cell_size;
  state_num_t *cell_array;
  short key_input;

  slk_init(1);

  if (initscr() == NULL) {
    return 0;
  }
  setup_cell_color(option->file_property.state_num);

  curs_set(0);
  noecho();
  cbreak();
  timeout(0);
  keypad(stdscr, TRUE);

  slk_set(8, "EXIT", 0);
  slk_refresh();

  if (create_status_window() == ERR) {
    return 0;
  }
  draw_status_window(option);

  cell_array = NULL;
  for (cell_size = option->cell_size; cell_size <= MAX_CELL_SIZE; cell_size++) {
    if (cell_array != NULL) {
      free(cell_array);
    }
    if ((cell_array = allocate_cell_array(cell_size)) == NULL) {
      return ERR_OUT_OF_MEMORY;
    }

    update_cell_count(cell_size);
    create_cell_window(cell_size, option->cell_width);
    refresh();

    ret = SUCCESS;
    for (step = 0; step <= 3 * cell_size; step++) {
      update_step_count(step);
      update_cell_window(step, cell_array, cell_size, option->cell_width);
      draw_cell_window(0, 0);
      doupdate();

      if (ret == SYNCHRONIZE) {
      	add_sync_count(1);
	doupdate();
	usleep(option->interval * 1000);
      	break;
      }
      if (ret == ERR_NOT_SYNCHRONIZE) {
      	add_not_sync_count(1);
	usleep(option->interval * 1000);
	doupdate();
      	break;
      }
      if (ret == ERR_UNDEFINED_RULE) {
      	add_undefined_count(1);
	usleep(option->interval * 1000);
	doupdate();
      	break;
      }

      key_input = wgetch(stdscr);
      switch (key_input) {
      case KEY_RESIZE:
	resize_status_window();
	draw_status_window(option);
	draw_cell_window(0, 0);
	break;
      case KEY_F(8):
	goto end;
      }

      ret = change_state(cell_array, cell_size + 2);
      usleep(option->interval * 1000);

    }
  }

 end: /* シグナル処理の時にまとめる */
  free_status_window();
  free_cell_window();
  free_state_color();
  free(cell_array);
  endwin();

  return SUCCESS;
}

static state_num_t* allocate_cell_array(size_t cell_size)
{
  int i;
  state_num_t *cell_array;

  if (SOLDIER == 0) {
    cell_array = (state_num_t *)calloc(sizeof(state_num_t), cell_size + 2);
  } else {
    cell_array = (state_num_t *)malloc(sizeof(state_num_t) * (cell_size + 2));
  }

  if (cell_array == NULL) {
    return NULL;
  }

  if (SOLDIER != 0) {
    for (i = 1; i <= cell_size; i++) {
      cell_array[i] = SOLDIER;
    }
  }
  cell_array[0] = cell_array[cell_size + 1] = EXTERNAL;
  cell_array[1] = GENERAL;

  return cell_array;
}
