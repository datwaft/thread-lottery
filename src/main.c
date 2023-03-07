#include <setjmp.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "deps/pcg_basic.h"

static void *coarg;

void *coroutine_goto(sigjmp_buf here, sigjmp_buf there, void *arg) {
  coarg = arg;
  if (sigsetjmp(here, false))
    return coarg;
  siglongjmp(there, 1);
}

#define STACK_DIRECTION -
#define STACK_SIZE (1 << 12)

static char *top_of_the_stack;

void *coroutine_start(sigjmp_buf here, void (*fun)(void *), void *arg) {
  if (top_of_the_stack == NULL)
    top_of_the_stack = (char *)&arg;
  top_of_the_stack += STACK_DIRECTION STACK_SIZE;
  char n[STACK_DIRECTION(top_of_the_stack - (char *)&arg)];
  coarg = n;
  if (sigsetjmp(here, false))
    return coarg;
  fun(arg);
  abort();
}

#define MAX_THREAD_NUMBER 10

static sigjmp_buf threads[MAX_THREAD_NUMBER];
static int n_threads = 0;

static void comain(void *arg) {
  int *p = arg, i = *p;
  for (;;) {
    printf("coroutine %d at %p with %p\n", i, (void *)&i, arg);
    int n = pcg32_boundedrand(n_threads);
    printf("jumping to %d\n", n);
    arg = coroutine_goto(threads[i], threads[n], (char *)arg + 1);
  }
}

int main(void) {
  pcg32_srandom(69, 420);

  while (++n_threads < MAX_THREAD_NUMBER) {
    printf("spawning %d\n", n_threads);
    coroutine_start(threads[0], comain, &n_threads);
  }
  return 0;
}
