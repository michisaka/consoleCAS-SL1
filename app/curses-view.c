#include <curses.h>
#include <stdlib.h>
#include <unistd.h>

#include "cassl1.h"
#include "application.h"
#include "curses-view.h"
#include "version.h"

static int setup_color(state_num_t state_num);
static short find_color_index(unsigned char color[]);
static short find_color_pair(short fg, short bg);
static int put_cell(state_num_t state_index);

static short *state_color;
static short last_color_pair = 16;

int start_curses_view(const option *option)
{
  int i;
  short key_input;
  WINDOW *cell_wnd;

  if (initscr() == NULL) {
    return 0;
  }
  setup_color(option->file_property.state_num);

  noecho();
  cbreak();
  timeout(0);

  if (create_status_window() == ERR) {
    return 0;
  }
  draw_status_window(option);

  cell_wnd = subwin(stdscr, 10, 20, 5, 0);
  box(cell_wnd, 0, 0);
  scrollok(cell_wnd, TRUE);
  idlok(cell_wnd, TRUE);
  mvwprintw(cell_wnd, 0, 2, " DEBUG ");

  refresh();

  for (i = 0; i < 20; i++) {
    mvwprintw(cell_wnd, 4, 1, "%2d (%d,%d)", i, COLS, LINES);
    update_step_count(i);
    add_sync_count(i * 2);

    key_input = wgetch(stdscr);
    if (key_input == KEY_RESIZE) {
      mvwprintw(cell_wnd, 5, 1, "RESIZED    ");
      resize_status_window();
      draw_status_window(option);

    } else {
      mvwprintw(cell_wnd, 5, 1, "NOT RESIZED");
    }
    wrefresh(cell_wnd);
    doupdate();
    usleep(1000*1000);
  }

  mvwprintw(cell_wnd, 7, 3, "END");
  refresh();

  free_status_window();
  free(state_color);
  endwin();

  return SUCCESS;
}

static int setup_color(state_num_t state_num)
{
  int i;
  short fg, bg;
  state *state;

  start_color();
  last_color_pair = 16;
  if ((state_color = (short*)calloc(sizeof(short), state_num)) == NULL) {
    return ERR_OUT_OF_MEMORY;
  }
  for (i = 0; i < state_num; i++) {
    state = get_state_byindex(i);
    fg = find_color_index(state->fg_rgb_color);
    bg = find_color_index(state->bg_rgb_color);
    state_color[i] = find_color_pair(fg, bg);
  }
  return SUCCESS;
}

static short find_color_index(unsigned char *color)
{
  short i;
  short r, g, b;

  short index;
  short delta, delta_tmp;

  index = COLORS;
  delta = 1000 * 3;

  for (i = COLORS - 1 ; i >= 0; i--) {
    color_content(i, &r, &g, &b);
    delta_tmp  = abs(r - (int)(color[0] * 1000.0 / 255.0));
    delta_tmp += abs(g - (int)(color[1] * 1000.0 / 255.0));
    delta_tmp += abs(b - (int)(color[2] * 1000.0 / 255.0));
    if (delta_tmp <= delta) {
      delta = delta_tmp;
      index = i;
    }
  }
  return index;
}

static short find_color_pair(short state_fg, short state_bg)
{
  int i, j;
  short fg, bg;

  /*  if (has_colors() && can_change_color()) {  */

  for (i = 0; i < COLOR_PAIRS; i++) {
    pair_content(i, &fg, &bg);
    if (fg == state_fg && bg == state_bg) {
      return i;
    }
  }

  if (last_color_pair < COLOR_PAIRS) {
    init_pair(last_color_pair++, state_fg, state_bg);
    return last_color_pair - 1;
  } else {
    return 0;
  }
}

static int put_cell(state_num_t state_index) {
  state *state;

  state = get_state_byindex(state_index);

  if (state == NULL) {
    return 0;
  }

  attron(COLOR_PAIR(state_color[state_index]));
  printw("%c ", state->name[0]);
  attroff(COLOR_PAIR(10));

  return SUCCESS;
}
