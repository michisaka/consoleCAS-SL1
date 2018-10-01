#if !defined(CURSES_VIEW_H_INCLUDED)
#define CURSES_VIEW_H_INCLUDED

#include "cassl1.h"
#include "application.h"

int create_status_window(void);
int resize_status_window(void);
int draw_status_window(const option *option);
int update_fileinfo(const option *option);
int add_sync_count(const int increment);
int add_not_sync_count(const int increment);
int add_not_fire_count(const int increment);
int add_undefined_count(const int increment);
int update_step_count(const int new_value);
int update_cell_count(const int new_value);
int update_general_count(const int new_value);
void free_status_window(void);

int setup_cell_color(state_num_t state_num);
int create_cell_window(int cell_size);
int draw_cell_window(int top, int left);
int update_cell_window(int step, const state_num_t *cell_array, int cell_size );
void free_cell_window(void);
void free_state_color(void);

#endif /* CURSES_VIEW_H_INCLUDED */
