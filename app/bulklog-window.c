#include <curses.h>
#include <pthread.h>

#include "cassl1.h"

static WINDOW *bulklog_wnd;

extern pthread_mutex_t curses_lock;

int create_bulklog_window(void)
{
  pthread_mutex_lock(&curses_lock);

  if ((bulklog_wnd = subwin(stdscr, LINES - 5, COLS, 5, 0)) == NULL) {
    pthread_mutex_unlock(&curses_lock);
    return ERR;
  }

  idlok(bulklog_wnd, TRUE);
  scrollok(bulklog_wnd, TRUE);

  pthread_mutex_unlock(&curses_lock);
  return SUCCESS;
}

int resize_bulklog_window(void)
{
  pthread_mutex_lock(&curses_lock);

  if (wresize(bulklog_wnd, LINES - 5, COLS) == ERR) {
    pthread_mutex_unlock(&curses_lock);
    return ERR;
  }

  pthread_mutex_unlock(&curses_lock);
  return SUCCESS;
}

void add_bulklog_message(char *msg)
{
  pthread_mutex_lock(&curses_lock);

  wprintw(bulklog_wnd, "%s\n", msg);
  wnoutrefresh(bulklog_wnd);

  pthread_mutex_unlock(&curses_lock);
}

void add_bulklog_result(int cell, int gpos, char *msg)
{
  pthread_mutex_lock(&curses_lock);

  wprintw(bulklog_wnd, "CELL %4d, GPOS, %4d %s\n", cell, gpos,  msg);
  wnoutrefresh(bulklog_wnd);

  pthread_mutex_unlock(&curses_lock);
}

void free_bulklog_window(void)
{
  pthread_mutex_lock(&curses_lock);

  if (bulklog_wnd != NULL) {delwin(bulklog_wnd); bulklog_wnd = NULL;}

  pthread_mutex_unlock(&curses_lock);
}
