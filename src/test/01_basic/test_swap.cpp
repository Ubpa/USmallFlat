#include "doctest.hpp"
#include <USmallFlat/small_vector.hpp>
using doctest::test_suite;
using namespace Ubpa;
#include <string>

TEST_CASE("Swap a pair of small vectors" * test_suite("swap")) {
  small_vector<int, 5> a{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  small_vector<int, 5> b;
  a.swap(b);
  REQUIRE(a.size() == 0);
  REQUIRE(b.size() == 10);
  std::size_t j = 0;
  for (int i = 0; i < 10; i++, j++) {
    REQUIRE(b[i] == i + 1);
  }
  REQUIRE(j == 10);
}