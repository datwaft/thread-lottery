#include "foo.h"
#include <stdio.h>
#include <stdlib.h>

#include "deps/xorshift128plus.c"

int main(int argc, char **argv) {
  s[0] = 69;
  s[1] = 420;

  for (int c = 'A'; c < 'A' + 26; c++) {
    // Colors
    // 31 = RED, 36 = CYAN
    int color = next() % (36 - 31 + 1) + 31;
    // Styles
    int style = next() % (7 + 1);
    printf("\x1b[%d;%dm"
           "%c"
           "\x1b[0m",
           style, color, c);
  }
  putchar('\n');
  putchar('\n');

  int x = foo(3);
  printf("%d\n", x);

  exit(0);
}
