#include <gtest/gtest.h>
#include <is_prime_lib/is_prime.h>

TEST(IsPrimeFast, Two) {
  ASSERT_TRUE(IsPrimeFast(2));
}

