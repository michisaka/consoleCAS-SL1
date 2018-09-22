#include <stdio.h>
#include <stdlib.h>
#include <libgen.h>
#include <unistd.h>
#include <string.h>

#include "cassl1.h"
#include "version.h"

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
  file_property file_prop;

  progname = argv[0];

  while ((ch = getopt(argc, argv, "hc:i:")) != -1) {
    switch (ch) {
    case 'c':
      cell_size = strtol(optarg, NULL, 0);
      break;
    case 'i':
      interval = strtol(optarg, NULL, 0);
      break;
    case '?':
    case 'h':
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

  printf("consoleCAS-SL1 version %d.%02d (%s-%s)\n",
	 VERSION_MAJOR, VERSION_MINOR, REVISION, BUILD_DATE);
  printf("  Copyright (c) 2018 Koshi.Michisaka\n\n");

  printf("Load %s\n", argv[0]);
  if ((ret =load_rulefile(argv[0], &file_prop)) != SUCCESS) {
    printf("Fail to load %s. return by %d\n", argv[0], ret);
    return EXIT_FAILURE;
  }

  strcpy(file_prop.path, basename(argv[0]));

  printf("file: %s, states: %d, rules: %ld\n", file_prop.path, file_prop.state_num, file_prop.rule_num);

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
  fprintf(stderr, "consoleCAS-SL1 version %d.%02d (%s-%s)\n\n",
	  VERSION_MAJOR, VERSION_MINOR, REVISION, BUILD_DATE
 );
  fprintf(stderr, "usage: %s [-h] [-i ms] [-c num] rulefile\n", basename(progname));
  fprintf(stderr, "    -h      show this message\n");
  fprintf(stderr, "    -c num  cell size\n");
  fprintf(stderr, "    -i ms   step interval\n\n");
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
