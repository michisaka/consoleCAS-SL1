#include <stdio.h>
#include <stdlib.h>
#include <libgen.h>
#include <unistd.h>
#include <string.h>

#include "cassl1.h"
#include "application.h"
#include "version.h"

static void usage(char *progname);

int main(int argc, char **argv)
{
  char progname[256];
  int ch;
  int ret;
  unsigned int cell_size = 0;
  unsigned int loop_end = 0;
  unsigned int cell_width = 2;
  unsigned int interval = 100;

  option option;

  strncpy(progname, basename(argv[0]), sizeof(progname));

  while ((ch = getopt(argc, argv, "hc:l::w:i:")) != -1) {
    switch (ch) {
    case 'c':
      cell_size = strtol(optarg, NULL, 0);
      break;
    case 'l':
      if (optarg == NULL) {
	loop_end = 1000000;
      } else {
	loop_end = strtol(optarg, NULL, 0);
      }
      break;
    case 'w':
      cell_width = strtol(optarg, NULL, 0);
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

  if (cell_size == 0) {
    cell_size = loop_end == 0 ? 10 : 2;
  }

  if (loop_end == 0) {
    loop_end = cell_size;
  }

  if (cell_size < 2) {
    printf("cell_size %d is too short.\n", cell_size);
    return EXIT_FAILURE;
  }

  if (loop_end < cell_size) {
    printf("loop_end %d is too short.\n", loop_end);
    return EXIT_FAILURE;
  }

  if (cell_width < 2 || cell_width > 6) {
    printf("cell_width must be between 2 and 6.\n");
    return EXIT_FAILURE;
  }

  if (interval < 1) {
    printf("minimal interval is 1.\n");
    return EXIT_FAILURE;
  }

  printf("consoleCAS-SL1 version %d.%02d (%s-%s)\n",
	 VERSION_MAJOR, VERSION_MINOR, REVISION, BUILD_DATE);
  printf("  Copyright (c) 2018 Koshi.Michisaka\n\n");

  printf("Load %s\n", argv[0]);
  if ((ret =load_rulefile(argv[0], &option.file_property)) != SUCCESS) {
    printf("Fail to load %s. return by %d\n", argv[0], ret);
    return EXIT_FAILURE;
  }

  strcpy(option.file_property.path, basename(argv[0]));
  option.cell_size = cell_size;
  option.loop_end = loop_end;
  option.cell_width = cell_width;
  option.interval = interval;

  switch (2) {
  case 1:
    ret = start_simple_view(&option);
    break;
  case 2:
    ret = start_curses_view(&option);
    break;
  }
  switch (ret) {

  case ERR_OUT_OF_MEMORY:
    printf("out of memory\n");
    break;
  case ERR_CURSES_ERROR:
    printf("curses error.\n");
    break;
  }

  free_states();
  free_ruleset();

  return ret == SUCCESS ? EXIT_SUCCESS : EXIT_FAILURE;
}

static void usage(char *progname)
{
  fprintf(stderr, "consoleCAS-SL1 version %d.%02d (%s-%s)\n\n",
	  VERSION_MAJOR, VERSION_MINOR, REVISION, BUILD_DATE);
  fprintf(stderr, "usage: %s [-h] [-c num] [-l[end]] [-w num] [-i ms] rulefile\n", basename(progname));
  fprintf(stderr, "    -h      show this message\n");
  fprintf(stderr, "    -c num  cell size\n");
  fprintf(stderr, "    -l[end] loop execution\n");
  fprintf(stderr, "    -w num  cell width (2-6)\n");
  fprintf(stderr, "    -i ms   step interval\n\n");

  return;
}
