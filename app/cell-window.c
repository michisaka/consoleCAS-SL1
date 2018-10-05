#include <curses.h>
#include <stdlib.h>
#include "string.h"

#include "cassl1.h"
#include "curses-view.h"

static short find_color_index(unsigned char color[]);
static short find_color_pair(short fg, short bg);
static int put_cell(state_num_t state_index, int cell_width);

static short *state_color;
static short last_color_pair = 16;

static WINDOW *cell_wnd;
static WINDOW *step_wnd;
static WINDOW *ruler_wnd;

int create_cell_window(int cell_size, int cell_width)
{
  int i;

  if (cell_wnd  != NULL) delwin(cell_wnd);
  if (step_wnd  != NULL) delwin(step_wnd);
  if (ruler_wnd != NULL) delwin(ruler_wnd);

  if ((cell_wnd  = newpad(3 * cell_size + 1, cell_size * cell_width)) == NULL) return ERR;
  if ((step_wnd  = newpad(3 * cell_size + 1, 5 )) == NULL) return ERR;
  if ((ruler_wnd = newpad(1, cell_size * cell_width + 10)) == NULL) return ERR;

  wmove(step_wnd, 0, 4);
  wvline(step_wnd, '|', 3 * cell_size + 1);
  mvwprintw(step_wnd, 0, 0, "0");

  mvwprintw(ruler_wnd, 0, 0, "1");
  for(i = 9; i < cell_size; i += 10) {
    mvwprintw(ruler_wnd, 0, i * cell_width, "%d", i + 1);
  }

  return SUCCESS;
}

int draw_cell_window(int top, int left)
{
  pnoutrefresh(step_wnd,  top, 0   , 6, 0, LINES - 1, 4);
  pnoutrefresh(ruler_wnd, 0  , left, 5, 5, 5        , COLS - 1);
  pnoutrefresh(cell_wnd,  top, left, 6, 5, LINES - 1, COLS - 1);
  return SUCCESS;
}

int update_cell_window(int step, const state_num_t *cell_array, int cell_size, int cell_width)
{
  int i;
  mvwprintw(step_wnd, step, 0, "%4d", step);
  for (i = 1; i <= cell_size; i++) {
    wmove(cell_wnd, step , (i - 1) * cell_width);
    put_cell(cell_array[i], cell_width);
  }
  return SUCCESS;
}

void free_cell_window(void)
{
  delwin(cell_wnd); cell_wnd = NULL;
  delwin(step_wnd); step_wnd = NULL;
  delwin(ruler_wnd); ruler_wnd = NULL;
}

int setup_cell_color(state_num_t state_num)
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

void free_state_color(void)
{
  free(state_color);
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

static int put_cell(state_num_t state_index, int cell_width) {
  state *state;
  char buf[6];

  state = get_state_byindex(state_index);

  if (state == NULL) {
    return 0;
  }

  strncpy(buf, state->name, cell_width - 1);
  buf[cell_width - 1] = '\0';
  if (state_color != NULL) {
    wattron(cell_wnd, COLOR_PAIR(state_color[state_index]));
  }
  wprintw(cell_wnd, "%s ", buf);
  wattroff(cell_wnd, COLOR_PAIR(10));

  return SUCCESS;
}
