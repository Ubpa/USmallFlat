#include "doctest.hpp"
#include <fixed_flat_set.h>
using doctest::test_suite;
using namespace Ubpa;
#include <string>

TEST_CASE("fixed flat set" * test_suite("all")) {
  static_assert(sizeof(fixed_flat_set<int, 5>) == sizeof(fixed_vector<int, 5>));

  //////////////////////
  // Member functions //
  //////////////////////
  {
    fixed_flat_set<int, 5> v;
    REQUIRE(v.empty());
  }
  {
    fixed_flat_set<int, 5> v;
    v.insert(1);
    v.insert(2);
    v.insert(1);
    REQUIRE(v == fixed_flat_set<int, 5>{1, 2});
  }
  {
    fixed_flat_set<int, 5> v{1,2,3,1,2};
    REQUIRE(v == fixed_flat_set<int, 5>{1, 2, 3});
  }
  {
    fixed_flat_set<int, 5> v{1,2,3};
    v.insert(v.begin(), 5);
    REQUIRE(v == fixed_flat_set<int, 5>{1, 2, 3, 5});
    v.insert(v.begin(), 5);
    REQUIRE(v == fixed_flat_set<int, 5>{1, 2, 3, 5});
    v.insert(v.begin() + 3, 4);
    REQUIRE(v == fixed_flat_set<int, 5>{1, 2, 3, 4, 5});
  }
  {
    fixed_flat_set<int, 5> v{1,2,3};
    v.erase(2);
    REQUIRE(v == fixed_flat_set<int, 5>{1, 3});
  }
  {
    fixed_flat_set<int, 5> v{1,2,4};
    auto iter = v.lower_bound(3);
    REQUIRE(iter == v.begin() + 2);
    v.insert(iter, 3);
    REQUIRE(v == fixed_flat_set<int, 5>{1, 2, 3, 4});
  }
}
