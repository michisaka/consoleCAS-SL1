#include <stdio.h>
#include <stdlib.h>
#include <libgen.h>
#include <unistd.h>

#include "cassl1.h"

static void usage(char *progname);
static int print_array(const state_num_t *cell_array, const int cell_size);

int main(int argc, char **argv)
{
  char *progname;
  int ch;
  int ret;
  int step;
  unsigned int cell_size = 10;
  state_num_t *cell_array;
  unsigned int interval = 100;

  progname = argv[0];

  while ((ch = getopt(argc, argv, "c:i:")) != -1) {
    switch (ch) {
    case 'c':
      cell_size = strtol(optarg, NULL, 0);
      break;
    case 'i':
      interval = strtol(optarg, NULL, 0);
      break;
    case '?':
    default:
      usage(progname);
      return EXIT_FAILURE;
    }
  }
  argc -= optind;
  argv += optind;

  if (argc != 1) {
    usage(progname);
    return EXIT_FAILURE;
  }

  if (cell_size < 2) {
    printf("cell_size %d is too short.\n", cell_size);
    return EXIT_FAILURE;
  }

  printf("Load %s\n", argv[0]);
  if ((ret =load_rulefile(argv[0])) != SUCCESS) {
    printf("Fail to load %s. return by %d\n", argv[0], ret);
    return EXIT_FAILURE;
  }

  cell_size += 2;

  if (SOLDIER == 0) {
    cell_array = (state_num_t *)calloc(sizeof(state_num_t), cell_size);
  } else {
    int i;
    cell_array = (state_num_t *)malloc(sizeof(state_num_t) * cell_size);
    for (i = 2; i < cell_size - 1; i++) {
      cell_array[i] = SOLDIER;
    }
  }
  if (cell_array == NULL) {
    printf("OUT_OF_MEMORY\n");
    return EXIT_FAILURE;
  }
  cell_array[0] = cell_array[cell_size -1] = EXTERNAL;
  cell_array[1] = GENERAL;

  print_array(cell_array, cell_size);
  for (step = 0; step < cell_size * 3; step++) {
    if (interval > 0) {
      usleep(interval * 100);
    }
    ret =change_state(cell_array, cell_size);
    print_array(cell_array, cell_size);
    if (ret != SUCCESS) {
      break;
    }
  }

  free_states();
  free_ruleset();
  return EXIT_SUCCESS;
}

static void usage(char *progname)
{
  fprintf(stderr, "usage: %s [-i ms -c num] rulefile\n", basename(progname));
  fprintf(stderr, "    -c num  cell size\n");
  fprintf(stderr, "    -i ms   step interval\n");
}

static int print_array(const state_num_t *cell_array, const int cell_size)
{
  int i;
  state *state;
  for (i = 1; i < cell_size - 1; i++) {
    state = get_state_byindex(cell_array[i]);
    if (state == NULL) {
      return ERR_UNDEFINED_STATE;
    }
    printf("\x1b[38;2;%d;%d;%dm\x1b[48;2;%d;%d;%dm%c \x1b[0m",
    	   state->fg_rgb_color[0], state->fg_rgb_color[1], state->fg_rgb_color[2],
    	   state->bg_rgb_color[0], state->bg_rgb_color[1], state->bg_rgb_color[2],
    	   state->name[0]);
  }
  putchar('\n');
  return SUCCESS;
}
