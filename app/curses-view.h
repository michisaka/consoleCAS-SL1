#if !defined(CURSES_VIEW_H_INCLUDED)
#define CURSES_VIEW_H_INCLUDED

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

#endif /* CURSES_VIEW_H_INCLUDED */
