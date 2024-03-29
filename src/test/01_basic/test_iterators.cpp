#include "doctest.h"
#include <USmallFlat/small_vector.hpp>
using doctest::test_suite;
using namespace Ubpa;
#include <string>

TEST_CASE("Forward iterate small vector of ints" * test_suite("iterators")) {
  std::vector<int> expected;
  small_vector<int, 5> foo{1, 2, 3};
  for (auto& v : foo) {
    expected.push_back(v);
  }

  REQUIRE(expected[0] == 1);
  REQUIRE(expected[1] == 2);
  REQUIRE(expected[2] == 3);

  expected.clear();
  for (const auto& v : foo) {
    expected.push_back(v);
  }

  REQUIRE(expected[0] == 1);
  REQUIRE(expected[1] == 2);
  REQUIRE(expected[2] == 3);
}
