#include "doctest.hpp"

#include <flat_set.h>
#include <fixed_vector.h>
#include <small_vector.h>

#include <fixed_flat_set.h>
#include <small_flat_set.h>
#include <flat_set.h>
#include <pmr/flat_set.h>

#include <fixed_flat_multiset.h>
#include <small_flat_multiset.h>
#include <flat_multiset.h>
#include <pmr/flat_multiset.h>

using doctest::test_suite;
using namespace Ubpa;
#include <string>
#include <memory_resource>

TEST_CASE("fixed flat set" * test_suite("all")) {
  static_assert(sizeof(fixed_flat_set<int, 5>) == sizeof(fixed_vector<int, 5>));
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
  //---
  {
    small_flat_set<int, 5> v;
    REQUIRE(v.empty());
  }
  {
    small_flat_set<int, 5> v;
    v.insert(1);
    v.insert(2);
    v.insert(1);
    REQUIRE(v == small_flat_set<int, 5>{1, 2});
  }
  {
    small_flat_set<int, 5> v{1,2,3,1,2};
    REQUIRE(v == small_flat_set<int, 5>{1, 2, 3});
  }
  {
    small_flat_set<int, 5> v{1,2,3};
    v.insert(v.begin(), 5);
    REQUIRE(v == small_flat_set<int, 5>{1, 2, 3, 5});
    v.insert(v.begin(), 5);
    REQUIRE(v == small_flat_set<int, 5>{1, 2, 3, 5});
    v.insert(v.begin() + 3, 4);
    REQUIRE(v == small_flat_set<int, 5>{1, 2, 3, 4, 5});
  }
  {
    small_flat_set<int, 5> v{1,2,3};
    v.erase(2);
    REQUIRE(v == small_flat_set<int, 5>{1, 3});
  }
  {
    small_flat_set<int, 5> v{1,2,4};
    auto iter = v.lower_bound(3);
    REQUIRE(iter == v.begin() + 2);
    v.insert(iter, 3);
    REQUIRE(v == small_flat_set<int, 5>{1, 2, 3, 4});
  }
  //--
  {
    pmr::flat_set<int> v;
    REQUIRE(v.empty());
  }
  {
    pmr::flat_set<int> v;
    v.insert(1);
    v.insert(2);
    v.insert(1);
    REQUIRE(v == pmr::flat_set<int>{1, 2});
  }
  {
    pmr::flat_set<int> v{1,2,3,1,2};
    REQUIRE(v == pmr::flat_set<int>{1, 2, 3});
  }
  {
    pmr::flat_set<int> v{1,2,3};
    v.insert(v.begin(), 5);
    REQUIRE(v == pmr::flat_set<int>{1, 2, 3, 5});
    v.insert(v.begin(), 5);
    REQUIRE(v == pmr::flat_set<int>{1, 2, 3, 5});
    v.insert(v.begin() + 3, 4);
    REQUIRE(v == pmr::flat_set<int>{1, 2, 3, 4, 5});
  }
  {
    pmr::flat_set<int> v{1,2,3};
    v.erase(2);
    REQUIRE(v == pmr::flat_set<int>{1, 3});
  }
  {
    pmr::flat_set<int> v{1,2,4};
    auto iter = v.lower_bound(3);
    REQUIRE(iter == v.begin() + 2);
    v.insert(iter, 3);
    REQUIRE(v == pmr::flat_set<int>{1, 2, 3, 4});
  }
  {
    fixed_vector<int, 5> s{1,2,4};
    fixed_flat_set<int, 5> v{ s };
    auto iter = v.lower_bound(3);
    v.insert(iter, 3);
    REQUIRE(v == fixed_flat_set<int, 5>{1, 2, 3, 4});
  }
  //--
  {
    fixed_flat_multiset<int, 5> v{ 1,1,2,3 };
    v.insert(2);
    REQUIRE(v == fixed_flat_multiset<int, 5>{1, 1, 2, 2, 3});
  }
}
