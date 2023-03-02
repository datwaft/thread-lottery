#include "foo.h"
#include <criterion/criterion.h>

Test(foo_tests, execution) {
  int result = foo(1);
  cr_expect_eq(result, 6);
}
