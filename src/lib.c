#include "lib.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

// Create a copy of the input string reversed.
// # Memory allocation:
// This function allocates the same amount of bytes as the input string.
char *reverse(const char *string) {
  size_t len = strlen(string);
  char *res = malloc(len);
  for (size_t i = len - 1, j = 0; string[i] != '\0'; i -= 1, j += 1) {
    res[j] = string[i];
  }
  return res;
}
