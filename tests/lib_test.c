#include "lib.h"
#include <criterion/criterion.h>

Test(reverse, works_with_odd_string) {
  char *res = reverse("12345");
  cr_expect_str_eq(res, "54321");
}
