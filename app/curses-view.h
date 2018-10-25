#if !defined(CURSES_VIEW_H_INCLUDED)
#define CURSES_VIEW_H_INCLUDED

#include "cassl1.h"
#include "application.h"

int init_curses(void);
state_num_t* allocate_cell_array(size_t cell_size);
void cleanup_curses(void);
void signal_handler(int sig);

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
int create_cell_window(int cell_size, int cell_width);
int draw_cell_window(int top, int left);
int update_cell_window(int step, const state_num_t *cell_array, int cell_size, int cell_width );
void free_cell_window(void);
void free_state_color(void);

int create_bulklog_window(void);
int resize_bulklog_window(void);
void add_bulklog_message(char *msg);
void add_bulklog_result(int cell, int gpos, char *msg);
void free_bulklog_window(void);

#endif /* CURSES_VIEW_H_INCLUDED */
