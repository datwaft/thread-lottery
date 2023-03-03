#include "foo.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "deps/pcg_basic.h"

int main(int argc, char *argv[]) {
  pcg32_srandom(69, 420);

  for (char c = 'A'; c < 'A' + ('Z' - 'A' + 1); c++) {
    // Colors
    uint32_t color = pcg32_boundedrand(36 - 31 + 1) + 31;
    // Styles
    uint32_t style = pcg32_boundedrand(7 + 1);
    printf("\x1b[%d;%dm"
           "%c"
           "\x1b[0m",
           style, color, c);
  }
  putchar('\n');
  putchar('\n');

  int32_t x = foo(3);
  printf("%d\n", x);

  return EXIT_SUCCESS;
}
