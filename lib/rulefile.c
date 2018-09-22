#include <stdio.h>
#include <string.h>

#include "cassl1.h"

static int load_formate_id(FILE *fp);
static int load_states(FILE *fp);
static int load_generals(FILE *fp);
static int load_rules(FILE *fp);

int load_rulefile(const char *path) {
  FILE *fp;
  int ret;

  if ((fp = fopen(path, "r")) == NULL) {
    return ERR_FILE_IO_ERROR;
  }

  if ((ret = load_formate_id(fp)) != SUCCESS) goto fail;
  if ((ret = load_states(fp))     != SUCCESS) goto fail;
  if ((ret = load_generals(fp))   != SUCCESS) goto fail;
  if ((ret = load_rules(fp))      != SUCCESS) goto fail;

  fclose(fp);
  return SUCCESS;

 fail:
  free_states();
  free_ruleset();
  fclose(fp);
  return ret;
}

static int load_formate_id(FILE *fp)
{
  char buf[256];
  char rule_id[8];

  if ((fgets(buf, sizeof(buf), fp) == NULL) || (sscanf(buf, "%7s", rule_id) != 1)) {
    return ERR_UNKNOWN_FORMAT;
  }

  if (strcmp(rule_id, "SL1rule") == 0) {
    printf("%s format.\n", rule_id);
    return SUCCESS;
  }
  return ERR_UNKNOWN_FORMAT;
}

static int load_states(FILE *fp)
{
  char buf[256];
  int i;

  int num;
  int ret;
  state state;
  char state_type[9];

  if ((fgets(buf, sizeof(buf), fp) == NULL) || (sscanf(buf,"state_number %d", &num) != 1)) {
    return ERR_UNKNOWN_FORMAT;
  }

  if (num < 4) {
    return ERR_TOO_SHORT;
  }

  printf("%d states: ", num);

  if ((ret = allocate_states(num)) != SUCCESS) {
    return ret;
  }

  if (RULECAP & RULECAP_ALLOCATE_BY_STATENUM) {
    if ((ret = allocate_ruleset(num)) != SUCCESS) {
      return ret;
    }
  }

  for (int i = 0; i < num; i++) {
    if (fgets(buf, sizeof(buf), fp) == NULL) {
      return ERR_FILE_IO_ERROR;
    }

    if (sscanf(buf, "%5c@%02hhx%02hhx%02hhx,%02hhx%02hhx%02hhx,%8s",
	       state.name,
	       &state.fg_rgb_color[2], &state.fg_rgb_color[1], &state.fg_rgb_color[0],
	       &state.bg_rgb_color[2], &state.bg_rgb_color[1], &state.bg_rgb_color[0],
	       state_type) != 8) {
      return ERR_UNKNOWN_FORMAT;
    }
    state.name[5] = '\0';

    if (strcmp(state_type, "others") == 0) {
      state.type = OTHERS;
    } else if (strcmp(state_type, "external") == 0) {
      state.type = EXTERNAL;
    } else if (strcmp(state_type, "soldier") == 0) {
      state.type = SOLDIER;
    } else if (strcmp(state_type, "general") == 0) {
      state.type = GENERAL;
    } else if (strcmp(state_type, "firing") == 0) {
      state.type = FIRING;
    } else {
      return ERR_UNKNOWN_FORMAT;
    }

    if ((ret = register_state(state)) != SUCCESS) {
      return ret;
    }

    if ((i + 1) == num) {
      printf("%d done.\n", i + 1);
    } else if ((i + 1) % 10) {
      putchar('.');
    } else {
      printf("%d", i + 1);
    }
  }

  return SUCCESS;
}

static int load_generals(FILE *fp)
{
  char buf[256];
  int i;

  int num;

  if ((fgets(buf, sizeof(buf), fp) == NULL) || (sscanf(buf,"general_number %d", &num) != 1)) {
    return ERR_UNKNOWN_FORMAT;
  }

  if (num < 1) {
    return ERR_TOO_SHORT;
  }

  printf("%d generals: ", num);

  for (int i = 0; i < num; i++) {
    if (fgets(buf, sizeof(buf), fp) == NULL) {
      return ERR_FILE_IO_ERROR;
    }

    if (sscanf(buf, "%*d,%*d") != 0) {
      return ERR_UNKNOWN_FORMAT;
    }

    if ((i + 1) == num) {
      printf("%d done.\n", i + 1);
    } else if ((i + 1) % 10) {
      putchar('.');
    } else {
      printf("%d", i + 1);
    }
  }

  return SUCCESS;
}

static int load_rules(FILE *fp)
{
  char buf[256];
  int i;

  int num;
  int ret;
  char left[6], center[6], right[6], next[6];

  if ((fgets(buf, sizeof(buf), fp) == NULL) || (sscanf(buf,"rule_number %d", &num) != 1)) {
    return ERR_UNKNOWN_FORMAT;
  }

  if (num < 1) {
    return ERR_TOO_SHORT;
  }

  printf("%d rules: ", num);

  if (!(RULECAP & RULECAP_ALLOCATE_BY_STATENUM)) {
    if ((ret = allocate_ruleset(num)) != SUCCESS) {
      return ret;
    }
  }

  for (int i = 0; i < num; i++) {
    if (fgets(buf, sizeof(buf), fp) == NULL) {
      return ERR_FILE_IO_ERROR;
    }

    if (sscanf(buf, "%5c##%5c##%5c->%5c",left, center, right, next) != 4) {
      return ERR_UNKNOWN_FORMAT;
    }
    left[5] = center[5] = right[5] = next[5] = '\0';

    if ((ret = register_rule(left, center, right, next)) != SUCCESS) {
      return ret;
    }

    if ((i + 1) == num) {
      printf("%d done.\n", i + 1);
    } else if ((i + 1) % 10) {
      putchar('.');
    } else {
      printf("%d", i + 1);
    }
  }

  return SUCCESS;
}
