#include <gtest/gtest.h>
#include <is_prime_lib/is_prime.h>

TEST(IsPrime, Two) {
  ASSERT_TRUE(IsPrime(2));
}

