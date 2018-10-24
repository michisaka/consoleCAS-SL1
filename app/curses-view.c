#include <curses.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>
#include <semaphore.h>

#include "cassl1.h"
#include "curses-view.h"

pthread_mutex_t curses_lock = PTHREAD_MUTEX_INITIALIZER;
sem_t keyinput_break_lock;

int init_curses(void)
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

state_num_t* allocate_cell_array(size_t cell_size)
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

void cleanup_curses(void)
{
  free_status_window();
  free_cell_window();
  free_state_color();
  endwin();
  return;
}

void signal_handler(int sig)
{
  cleanup_curses();
  exit(EXIT_FAILURE);
}

