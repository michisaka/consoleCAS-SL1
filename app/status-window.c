#include <curses.h>
#include <pthread.h>

#include "application.h"
#include "curses-view.h"
#include "version.h"

WINDOW *status_wnd;
WINDOW *fileinfo_wnd;
WINDOW *result_wnd;
WINDOW *arrayinfo_wnd;

extern pthread_mutex_t curses_lock;

int create_status_window(void)
{
  pthread_mutex_lock(&curses_lock);

  if ((status_wnd =    subwin(stdscr,     5, COLS,     0, 0)) == NULL) {
    pthread_mutex_unlock(&curses_lock);
    return ERR;
  }
  if ((fileinfo_wnd =  subwin(status_wnd, 1, COLS - 2, 1, 1)) == NULL) {
    pthread_mutex_unlock(&curses_lock);
    return ERR;
  }
  if ((result_wnd =    subwin(status_wnd, 1, COLS - 2, 2, 1)) == NULL) {
    pthread_mutex_unlock(&curses_lock);
    return ERR;
  }
  if ((arrayinfo_wnd = subwin(status_wnd, 1, COLS - 2, 3, 1)) == NULL) {
    pthread_mutex_unlock(&curses_lock);
    return ERR;
  }

  pthread_mutex_unlock(&curses_lock);
  return SUCCESS;
}

int resize_status_window(void)
{
  pthread_mutex_lock(&curses_lock);

  if (wresize(status_wnd,    5, COLS    ) == ERR) {
    pthread_mutex_unlock(&curses_lock);
    return ERR;
  }
  if (wresize(fileinfo_wnd,  1, COLS - 2) == ERR) {
    pthread_mutex_unlock(&curses_lock);
    return ERR;
  }
  if (wresize(result_wnd,    1, COLS - 2) == ERR) {
    pthread_mutex_unlock(&curses_lock);
    return ERR;
  }
  if (wresize(arrayinfo_wnd, 1, COLS - 2) == ERR) {
    pthread_mutex_unlock(&curses_lock);
    return ERR;
  }

  werase(status_wnd);

  pthread_mutex_unlock(&curses_lock);
  return SUCCESS;
}

int draw_status_window(const option *option)
{
  pthread_mutex_lock(&curses_lock);

  box(status_wnd, 0, 0);
  mvwprintw(status_wnd, 0, 2, " consoleCAS-SL1 v.%d.%02d (%s) ", VERSION_MAJOR, VERSION_MINOR, REVISION);

  mvwprintw(fileinfo_wnd, 0, 1,  "FILE:        STATES:       0  RULES:        0");
  mvwprintw(result_wnd, 0, 1,    "SYNC:     0  NOT_SYNC:     0  NOT_FIRE:     0  UNDEFINED:     0");
  mvwprintw(arrayinfo_wnd, 0, 1, "STEP:     0  CELL:         0  GENERAL:      0");
  pthread_mutex_unlock(&curses_lock);

  update_fileinfo(option);
  add_sync_count(0);
  add_not_sync_count(0);
  add_not_fire_count(0);
  add_undefined_count(0);
  update_step_count(-1);
  update_cell_count(-1);
  update_general_count(-1);

  pthread_mutex_lock(&curses_lock);
  wnoutrefresh(status_wnd);
  wnoutrefresh(fileinfo_wnd);
  wnoutrefresh(result_wnd);
  wnoutrefresh(arrayinfo_wnd);
  pthread_mutex_unlock(&curses_lock);

  return SUCCESS;
}

int update_fileinfo(const option *new_option)
{
  static option saved_value;

  if (new_option != NULL) {
    saved_value = *new_option;
  }

  pthread_mutex_lock(&curses_lock);

  mvwprintw(fileinfo_wnd, 0, 1, "FILE: %-22s  STATES:   %5d  RULES:     %5ld",
	    saved_value.file_property.path,
	    saved_value.file_property.state_num,
	    saved_value.file_property.rule_num);
  wnoutrefresh(fileinfo_wnd);

  pthread_mutex_unlock(&curses_lock);
  return SUCCESS;
}

int add_sync_count(const int increment)
{
  static int saved_value;

  if (increment >= 0) {
    saved_value += increment;
  } else if (increment == -1) {
    saved_value = 0;
  }

  pthread_mutex_lock(&curses_lock);

  mvwprintw(result_wnd, 0, 7, "%5d", saved_value);
  wnoutrefresh(result_wnd);

  pthread_mutex_unlock(&curses_lock);
  return saved_value;
}

int add_not_sync_count(const int increment)
{
  static int saved_value;

  if (increment >= 0) {
    saved_value += increment;
  } else if (increment == -1) {
    saved_value = 0;
  }

  pthread_mutex_lock(&curses_lock);

  mvwprintw(result_wnd, 0, 24, "%5d", saved_value);
  wnoutrefresh(result_wnd);

  pthread_mutex_unlock(&curses_lock);
  return saved_value;
}

int add_not_fire_count(const int increment)
{
  static int saved_value;

  if (increment >= 0) {
    saved_value += increment;
  } else if (increment == -1) {
    saved_value = 0;
  }

  pthread_mutex_lock(&curses_lock);

  mvwprintw(result_wnd, 0, 41, "%5d", saved_value);
  wnoutrefresh(result_wnd);

  pthread_mutex_unlock(&curses_lock);
  return saved_value;
}

int add_undefined_count(const int increment)
{
  static int saved_value;

  if (increment >= 0) {
    saved_value += increment;
  } else if (increment == -1) {
    saved_value = 0;
  }

  pthread_mutex_lock(&curses_lock);

  mvwprintw(result_wnd, 0, 59, "%5d", saved_value);
  wnoutrefresh(result_wnd);

  pthread_mutex_unlock(&curses_lock);
  return saved_value;
}

int update_step_count(const int new_value)
{
  static int saved_value;

  if (new_value >= 0) {
    saved_value = new_value;
  }

  pthread_mutex_lock(&curses_lock);

  mvwprintw(arrayinfo_wnd, 0, 7, "%5d", saved_value);
  wnoutrefresh(arrayinfo_wnd);

  pthread_mutex_unlock(&curses_lock);
  return saved_value;
}

int update_cell_count(const int new_value)
{
  static int saved_value;

  if (new_value >= 0) {
    saved_value = new_value;
  }

  pthread_mutex_lock(&curses_lock);

  mvwprintw(arrayinfo_wnd, 0, 24, "%5d", saved_value);
  wnoutrefresh(arrayinfo_wnd);

  pthread_mutex_unlock(&curses_lock);
  return saved_value;
}

int update_general_count(const int new_value)
{
  static int saved_value;

  if (new_value >= 0) {
    saved_value = new_value;
  }

  pthread_mutex_lock(&curses_lock);

  mvwprintw(arrayinfo_wnd, 0, 41, "%5d", saved_value);
  wnoutrefresh(arrayinfo_wnd);

  pthread_mutex_unlock(&curses_lock);
  return saved_value;
}

void free_status_window(void)
{
  pthread_mutex_lock(&curses_lock);

  if (arrayinfo_wnd != NULL) {delwin(arrayinfo_wnd); arrayinfo_wnd = NULL;}
  if (result_wnd != NULL)    {delwin(result_wnd);    result_wnd = NULL;}
  if (fileinfo_wnd != NULL)  {delwin(fileinfo_wnd);  fileinfo_wnd = NULL;}
  if (status_wnd != NULL)    {delwin(status_wnd);    status_wnd = NULL;}

  pthread_mutex_unlock(&curses_lock);
}
