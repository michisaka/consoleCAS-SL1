#include <curses.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <semaphore.h>

#include "cassl1.h"
#include "application.h"
#include "curses-view.h"

extern pthread_mutex_t curses_lock;
extern sem_t keyinput_break_lock;

static void* bulk_mode_thread(void *arg);
static void bulk_mode_thread_cleanup(void *arg);

int bulk_mode_main(const option *option)
{
  int ret;
  char msg[256];
  time_t start, finish;
  struct tm tm_buf;
  short key_input;
  pthread_t thread_id;

  if ((ret = init_curses()) != SUCCESS) {
    return ret;
  }

  if (create_status_window() == ERR ||
      create_bulklog_window() == ERR) {
    cleanup_curses();
    return ERR_CURSES_ERROR;
  }
  draw_status_window(option);

  time(&start);
  localtime_r(&start, &tm_buf);
  strftime(msg, sizeof(msg), "Bulk mode start on %c.", &tm_buf);
  add_bulklog_message(msg);
  add_bulklog_message("STEP is displayed with CELL >= 10000.\n");

  if (sem_init(&keyinput_break_lock, 0, 0) != 0) {
    cleanup_curses();
    return ERR_THREAD_ERROR;
  }

  if (pthread_create(&thread_id, NULL, bulk_mode_thread, (void*)option) != 0) {
    cleanup_curses();
    return ERR_THREAD_ERROR;
  }

  while (1) {
    pthread_mutex_lock(&curses_lock);
    key_input = wgetch(stdscr);
    pthread_mutex_unlock(&curses_lock);
    switch (key_input) {
    case KEY_RESIZE:
      if (resize_status_window() == ERR ||
	  resize_bulklog_window() == ERR) {
	pthread_cancel(thread_id);
	pthread_join(thread_id, NULL);
	pthread_mutex_destroy(&curses_lock);
	sem_destroy(&keyinput_break_lock);
	cleanup_curses();
	return ERR_CURSES_ERROR;
      }
      draw_status_window(option);
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
    usleep(1000);
  }
  pthread_cancel(thread_id);
  pthread_join(thread_id, (void **)&ret);

  if (ret == SUCCESS) {
    time(&finish);
    localtime_r(&finish, &tm_buf);
    strftime(msg, sizeof(msg), "\nFinish on %c.", &tm_buf);
    add_bulklog_message(msg);
    doupdate();
  }

  pthread_mutex_destroy(&curses_lock);
  sem_destroy(&keyinput_break_lock);

  cleanup_curses();
  return ret;
}

static void* bulk_mode_thread(void *arg)
{
  int ret;

  state_num_t *cell_array;
  int cell_size;
  int step;
  option *opt;

  opt = (option *)arg;

  pthread_cleanup_push(bulk_mode_thread_cleanup, &cell_array);

  cell_array = NULL;
  for (cell_size = opt->cell_size; cell_size <= opt->loop_end; cell_size++) {
    if (cell_array != NULL) {
      free(cell_array);
    }
    if ((cell_array = allocate_cell_array(cell_size)) == NULL) {
      sem_post(&keyinput_break_lock);
      pthread_exit((void*)ERR_OUT_OF_MEMORY);
    }
    update_cell_count(cell_size);
    refresh();

    ret = SUCCESS;
    for (step = 0; step <= 3 * cell_size; step++) {
      if ((cell_size >= 100000 && step % 100 == 0) ||
	  (cell_size >= 10000 && step % 1000 == 0)) {
	update_step_count(step);
      }

      switch (ret) {
      case SYNCHRONIZE:
      	add_sync_count(1);
	//	add_bulklog_result(cell_size, 0, "synchronized.");
	break;
      case ERR_NOT_SYNCHRONIZE:
      	add_not_sync_count(1);
	add_bulklog_result(cell_size, 0, "not synchronized.");
	break;
      case ERR_UNDEFINED_RULE:
      	add_undefined_count(1);
	add_bulklog_result(cell_size, 0,"undefined rule referred.");
	break;
      }
      pthread_mutex_lock(&curses_lock);
      doupdate();
      pthread_mutex_unlock(&curses_lock);

      pthread_testcancel();
      pthread_yield();

      if (ret != SUCCESS) {
	break;
      }
      ret = change_state(cell_array, cell_size + 2);
    }
    if (step > 3 * cell_size) {
      add_not_fire_count(1);
      add_bulklog_result(cell_size, 0,"not firing.");
      pthread_mutex_lock(&curses_lock);
      doupdate();
      pthread_mutex_unlock(&curses_lock);
    }
  }

  pthread_cleanup_pop(1);
  sem_post(&keyinput_break_lock);
  return (void*)SUCCESS;
}

static void bulk_mode_thread_cleanup(void *arg)
{
  state_num_t *cell_array;

  cell_array = *(state_num_t **)arg;
  if (cell_array != NULL) {
    free(cell_array);
  }
  pthread_mutex_unlock(&curses_lock);

}

