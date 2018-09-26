#if !defined(APPLICATION_H_INCLUDED)
#define APPLICATION_H_INCLUDED

#include "cassl1.h"

typedef struct option {
  unsigned int max_cell_size;
  unsigned int interval;

  file_property file_property;

} option;

int start_simple_view(const option *option);
int start_curses_view(const option *option);


#endif /* APPLICATION_H_INCLUDED */
