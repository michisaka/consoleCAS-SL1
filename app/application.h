#if !defined(APPLICATION_H_INCLUDED)
#define APPLICATION_H_INCLUDED

#include "cassl1.h"

typedef struct option {
  unsigned int cell_size;
  unsigned int loop_end;
  unsigned int cell_width;
  unsigned int interval;

  file_property file_property;

} option;

int start_simple_view(const option *option);
int visual_mode_main(const option *option);


#endif /* APPLICATION_H_INCLUDED */
