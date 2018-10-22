#include <curses.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>

#include "cassl1.h"
#include "application.h"
#include "curses-view.h"
#include "version.h"

typedef struct drawing_param {
  int cell_size;
  int step;
  int top;
  int left;
  const option *option;
} drawing_param;

pthread_mutex_t curses_lock = PTHREAD_MUTEX_INITIALIZER;

static int init_curses(void);
static state_num_t* allocate_cell_array(size_t cell_size);
static void cleanup_curses(void);
static void signal_handler(int sig);
static void* drawing_thread(void *arg);

int start_curses_view(const option *option)
{
  int ret;
  short key_input;

  pthread_t drawing_thread_id;
  drawing_param param;

  param.option = option;

  if ((ret = init_curses()) != SUCCESS) {
    return ret;
  }

  if ((ret = setup_cell_color(option->file_property.state_num)) != SUCCESS) {
    cleanup_curses();
    return ret;
  }

  if (create_status_window() == ERR) {
    cleanup_curses();
    return ERR_CURSES_ERROR;
  }
  draw_status_window(option);

  if (pthread_create(&drawing_thread_id, NULL, drawing_thread, (void*)&param) != 0) {
    cleanup_curses();
    return ERR_THREAD_REEOR;
  }

  while (1) {
    pthread_mutex_lock(&curses_lock);
    key_input = wgetch(stdscr);
    pthread_mutex_unlock(&curses_lock);
    switch (key_input) {
    case KEY_RESIZE:
      param.left = 0;
      param.top = 0;
      if (resize_status_window() == ERR) {
	cleanup_curses();
//        free(cell_array);
	return ERR_CURSES_ERROR;
      }
      draw_status_window(option);
      draw_cell_window(param.top, param.left);
      doupdate();
      break;
    case KEY_LEFT:
      if (param.left >= option->cell_width) {
	param.left -= option->cell_width;
      }
      draw_cell_window(param.top, param.left);
      doupdate();
      break;
    case KEY_RIGHT:
      if ((int)(param.cell_size * option->cell_width) -
	  (param.left + (COLS - 5)) >= option->cell_width) {
	param.left += option->cell_width;
      }
      draw_cell_window(param.top, param.left);
      doupdate();
      break;
    case KEY_UP:
      if (param.top > 0) {
	param.top -= 1;
      }
      draw_cell_window(param.top, param.left);
      doupdate();
      break;
    case KEY_DOWN:
      if (param.step - (param.top + (LINES - 7)) > 0) {
	param.top += 1;
      }
      draw_cell_window(param.top, param.left);
      doupdate();
      break;
    case KEY_F(8): /* EXIT */
      cleanup_curses();
//      free(cell_array);
      return SUCCESS;
    }
  }
  cleanup_curses();
//      free(cell_array);
  return 0;
}

static int init_curses(void)
{
  slk_init(1);

  if (initscr() == NULL) {
    return ERR_CURSES_ERROR;
  }

  signal(SIGINT,  signal_handler);
  signal(SIGTSTP, signal_handler);
  signal(SIGQUIT, signal_handler);

  curs_set(0);
  noecho();
  cbreak();
  timeout(0);
  keypad(stdscr, TRUE);

  slk_set(8, "EXIT", 0);
  slk_refresh();

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

static void cleanup_curses(void)
{
  free_status_window();
  free_cell_window();
  free_state_color();
  endwin();
  return;
}

static void signal_handler(int sig)
{
  cleanup_curses();
  exit(EXIT_FAILURE);
}

static void* drawing_thread(void *arg)
{
  int ret;

  state_num_t *cell_array;
  drawing_param *param;
  const option *option;
  param = (drawing_param*)arg;
  option = param->option;


  cell_array = NULL;
  for (param->cell_size = option->cell_size; param->cell_size <= option->loop_end; param->cell_size++) {
    if (cell_array != NULL) {
      free(cell_array);
    }
    if ((cell_array = allocate_cell_array(param->cell_size)) == NULL) {
      cleanup_curses();
      pthread_exit((void*)ERR_OUT_OF_MEMORY);
    }

    update_cell_count(param->cell_size);
    if ((create_cell_window(param->cell_size, option->cell_width)) != SUCCESS) {
      cleanup_curses();
      free(cell_array);
      pthread_exit((void*)ERR_OUT_OF_MEMORY);
    }
    refresh();

    param->top = 0;
    param->left = 0;
    ret = SUCCESS;

    for (param->step = 0; param->step <= 3 * param->cell_size; param->step++) {
      update_step_count(param->step);
      update_cell_window(param->step, cell_array, param->cell_size, option->cell_width);

      if (param->step - param->top == LINES - 6) {
	param->top++;
      }
      draw_cell_window(param->top, param->left);

      switch (ret) {
      case SYNCHRONIZE:
      	add_sync_count(1);
	break;
      case ERR_NOT_SYNCHRONIZE:
      	add_not_sync_count(1);
	break;
      case ERR_UNDEFINED_RULE:
      	add_undefined_count(1);
	break;
      }
      doupdate();
      usleep(1000 * option->interval);

      if (ret != SUCCESS) {
	break;
      }
      ret = change_state(cell_array, param->cell_size + 2);
    }
  }
  free(cell_array);
  return (void*)SUCCESS;
}
