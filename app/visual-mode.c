#include <curses.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#include "cassl1.h"
#include "application.h"
#include "curses-view.h"

typedef struct visual_mode_param {
  int cell_size;
  int step;
  int top;
  int left;
  const option *option;
} visual_mode_param;

extern pthread_mutex_t curses_lock;
extern sem_t keyinput_break_lock;

static void* visual_mode_thread(void *arg);
static void visual_mode_thread_cleanup(void *arg);

int visual_mode_main(const option *option)
{
  int ret;
  short key_input;

  pthread_t thread_id;
  visual_mode_param param;

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

  if (sem_init(&keyinput_break_lock, 0, 0) != 0) {
    cleanup_curses();
    return ERR_THREAD_ERROR;
  }

  if (pthread_create(&thread_id, NULL, visual_mode_thread, (void*)&param) != 0) {
    cleanup_curses();
    return ERR_THREAD_ERROR;
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
	pthread_cancel(thread_id);
	pthread_join(thread_id, NULL);
	pthread_mutex_destroy(&curses_lock);
	sem_destroy(&keyinput_break_lock);
	cleanup_curses();
	return ERR_CURSES_ERROR;
      }
      draw_status_window(option);
      draw_cell_window(param.top, param.left);
      pthread_mutex_lock(&curses_lock);
      doupdate();
      pthread_mutex_unlock(&curses_lock);
      break;
    case KEY_LEFT:
      if (param.left >= option->cell_width) {
	param.left -= option->cell_width;
      }
      draw_cell_window(param.top, param.left);
      pthread_mutex_lock(&curses_lock);
      doupdate();
      pthread_mutex_unlock(&curses_lock);
      break;
    case KEY_RIGHT:
      if ((int)(param.cell_size * option->cell_width) -
	  (param.left + (COLS - 5)) >= (int)option->cell_width) {
	param.left += option->cell_width;
      }
      draw_cell_window(param.top, param.left);
      pthread_mutex_lock(&curses_lock);
      doupdate();
      pthread_mutex_unlock(&curses_lock);
      break;
    case KEY_UP:
      if (param.top > 0) {
	param.top -= 1;
      }
      draw_cell_window(param.top, param.left);
      pthread_mutex_lock(&curses_lock);
      doupdate();
      pthread_mutex_unlock(&curses_lock);
      break;
    case KEY_DOWN:
      if (param.step - (param.top + (LINES - 7)) > 0) {
	param.top += 1;
      }
      draw_cell_window(param.top, param.left);
      pthread_mutex_lock(&curses_lock);
      doupdate();
      pthread_mutex_unlock(&curses_lock);
      break;
    case KEY_F(8): /* EXIT */
    case 'q':
      pthread_cancel(thread_id);
      pthread_join(thread_id, NULL);
      pthread_mutex_destroy(&curses_lock);
      sem_destroy(&keyinput_break_lock);
      cleanup_curses();
      return SUCCESS;
    }
    if (sem_trywait(&keyinput_break_lock) == 0) {
      break;
    }
    pthread_yield();
  }
  pthread_cancel(thread_id);
  pthread_join(thread_id, (void **)&ret);
  pthread_mutex_destroy(&curses_lock);
  sem_destroy(&keyinput_break_lock);

  cleanup_curses();
  return ret;
}

static void* visual_mode_thread(void *arg)
{
  int ret;

  state_num_t *cell_array;
  visual_mode_param *param;
  const option *option;
  param = (visual_mode_param*)arg;
  option = param->option;

  pthread_cleanup_push(visual_mode_thread_cleanup, &cell_array);

  cell_array = NULL;
  for (param->cell_size = option->cell_size; param->cell_size <= option->loop_end; param->cell_size++) {
    if (cell_array != NULL) {
      free(cell_array);
    }
    if ((cell_array = allocate_cell_array(param->cell_size)) == NULL) {
      sem_post(&keyinput_break_lock);
      pthread_exit((void*)ERR_OUT_OF_MEMORY);
    }

    update_cell_count(param->cell_size);
    if ((create_cell_window(param->cell_size, option->cell_width)) != SUCCESS) {
      sem_post(&keyinput_break_lock);
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
      pthread_mutex_lock(&curses_lock);
      doupdate();
      pthread_mutex_unlock(&curses_lock);
      pthread_testcancel();
      usleep(1000 * option->interval);

      if (ret != SUCCESS) {
	break;
      }
      ret = change_state(cell_array, param->cell_size + 2);
    }
    if (param->step > 3 * param->cell_size) {
      add_not_fire_count(1);
      pthread_mutex_lock(&curses_lock);
      doupdate();
      pthread_mutex_unlock(&curses_lock);
    }
  }
  pthread_cleanup_pop(1);
  return (void*)SUCCESS;
}

static void visual_mode_thread_cleanup(void *arg)
{
  state_num_t *cell_array;

  cell_array = *(state_num_t **)arg;
  if (cell_array != NULL) {
    free(cell_array);
  }
  pthread_mutex_unlock(&curses_lock);
}
