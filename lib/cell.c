#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include "cassl1.h"

#define DEFAULT_THREAD_NUM 8
#define MIN_BUCKET_SIZE 20

typedef enum thread_status {NOT_READY, READY} thread_status;
typedef struct thread_parameter{
  state_num_t *start_pos;
  size_t sub_array_size;
  state_num_t left_end;
  state_num_t right_end;
  thread_status status;
  int result;
  pthread_t thread_id;
} thread_parameter;


static pthread_once_t once_control = PTHREAD_ONCE_INIT;
static pthread_mutex_t status_lock;
static pthread_cond_t wait_cond_v;
static int done_thread;

static thread_parameter *thread_params;
static int thread_num = DEFAULT_THREAD_NUM;

static int translate_subarray(state_num_t *cell_array, size_t size, state_num_t left_end, state_num_t right_end);
static void initialize_thread(void);
static void* translation_thread(void *arg);

int translate_array(state_num_t *cell_array, size_t size)
{
  int i;
  int active_thread_num;
  int sub_array_size;
  int count_sync;
  state_num_t *start_pos;

  pthread_once(&once_control, initialize_thread);

  if (size < 3) {
    return ERR_TOO_SMALL;
  }

  if  ((size - 2) / MIN_BUCKET_SIZE + 1 > thread_num) {
    active_thread_num = thread_num;
  } else {
    active_thread_num = (size - 2) / MIN_BUCKET_SIZE + 1;
  }

  if (active_thread_num == 1) {
    return translate_subarray(cell_array + 1, size - 2 , cell_array[0], cell_array[size + 1]);
  }

  if (thread_params == NULL) {
    return ERR_THREAD_ERROR;
  }

  pthread_mutex_lock(&status_lock);

  start_pos = cell_array + 1;
  sub_array_size = (size - 2) / active_thread_num;

  for (i = 0; i < active_thread_num; i++) {
    thread_params[i].start_pos = start_pos;

    if (i == active_thread_num - 1) {
      sub_array_size += (size - 2) % active_thread_num;
    }
    thread_params[i].sub_array_size = sub_array_size;
    thread_params[i].left_end = *(start_pos - 1);
    thread_params[i].right_end = *(start_pos + sub_array_size);
    thread_params[i].status = READY;
    thread_params[i].result = 0;

    start_pos += sub_array_size;
  }
  done_thread = 0;
  pthread_cond_broadcast(&wait_cond_v);
  pthread_mutex_unlock(&status_lock);

  pthread_mutex_lock(&status_lock);
  while(done_thread != active_thread_num) {
    pthread_cond_wait(&wait_cond_v, &status_lock);
  }
  pthread_mutex_unlock(&status_lock);

  count_sync = 0;
  for (i = 0; i < active_thread_num; i++) {
    switch (thread_params[i].result) {
    case ERR_NOT_SYNCHRONIZE:
    case ERR_UNDEFINED_RULE:
      return thread_params[i].result;
    case SYNCHRONIZE:
      count_sync++;
    }
  }

  if (count_sync == 0) {
    return SUCCESS;
  } else if (count_sync == active_thread_num) {
    return SYNCHRONIZE;
  } else {
    return ERR_NOT_SYNCHRONIZE;
  }
}

static void initialize_thread(void)
{
  int i;
  int ret;
  thread_num = sysconf(_SC_NPROCESSORS_ONLN);

  if (thread_num == -1) {
    thread_num = DEFAULT_THREAD_NUM;
  }

  if (pthread_mutex_init(&status_lock, NULL) != 0) {
    return;
  }

  if (pthread_cond_init(&wait_cond_v, NULL) != 0) {
    return;
  }

  thread_params = (thread_parameter*)malloc(sizeof(thread_parameter) * thread_num);

  if (thread_params == NULL) {
    return;
  }

  for (i = 0; i < thread_num; i++) {
    thread_params[i].status = NOT_READY;
    ret = pthread_create(&(thread_params[i].thread_id), NULL, translation_thread, (void *)&thread_params[i]);
    if (ret != 0) {
      free(thread_params);
      thread_params = NULL;
      return;
    }
  }
}

static void* translation_thread(void *arg)
{
  thread_parameter *param = (thread_parameter *)arg;

  while (1) {
    pthread_mutex_lock(&status_lock);
    while (param->status != READY) {
      pthread_cond_wait(&wait_cond_v, &status_lock);
    }
    pthread_mutex_unlock(&status_lock);

    param->result = translate_subarray(param->start_pos,
				       param->sub_array_size,
				       param->left_end,
				       param->right_end);

    pthread_mutex_lock(&status_lock);
    param->status = NOT_READY;
    done_thread++;
    pthread_cond_broadcast(&wait_cond_v);
    pthread_mutex_unlock(&status_lock);
  }

  return NULL;
}


static int translate_subarray(state_num_t *cell_array, size_t size, state_num_t left_end, state_num_t right_end)
{
  state_num_t save;
  state_num_t next;

  int i;
  int count_undefined = 0;
  int count_firing = 0;

  for (i = 0, save = left_end; i < size; i++) {
    next = get_next_state(save, cell_array[i],
			  i == size - 1 ? right_end : cell_array[i + 1]);

    if (next == FIRING) {
      count_firing++;
    } else if (next == EXTERNAL) {
      count_undefined++;
    }

    save = cell_array[i];
    cell_array[i] = next;
  }

  if (count_undefined != 0) {
    return ERR_UNDEFINED_RULE;
  }

  if (count_firing != 0) {
    if (count_firing == size) {
      return SYNCHRONIZE;
    } else {
      return ERR_NOT_SYNCHRONIZE;
    }
  }
  return SUCCESS;
}
