#include <stdio.h>
#include <stdlib.h>

#include "scheduler.h"

struct tester_args {
  const char *name;
  int iters;
};

void tester(void *arg) {
  int i;
  struct tester_args *ta = (struct tester_args *)arg;
  for (i = 0; i < ta->iters; i++) {
    printf("task %s: %d\n", ta->name, i);
    scheduler_pause_current_task();
  }
  free(ta);
}

void create_test_task(const char *name, int iters) {
  struct tester_args *ta = malloc(sizeof(*ta));
  ta->name = name;
  ta->iters = iters;
  scheduler_create_task(tester, ta);
}

int main(int argc, char **argv) {
  scheduler_init();
  create_test_task("first", 5);
  create_test_task("second", 2);
  scheduler_run();
  printf("Finished running all tasks!\n");
  return EXIT_SUCCESS;
}
