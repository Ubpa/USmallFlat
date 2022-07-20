#include "doctest.h"

#include <USmallFlat/flat_set.hpp>
#include <USmallFlat/static_vector.hpp>
#include <USmallFlat/small_vector.hpp>

#include <USmallFlat/static_flat_set.hpp>
#include <USmallFlat/small_flat_set.hpp>
#include <USmallFlat/flat_set.hpp>
#include <USmallFlat/pmr/flat_set.hpp>

#include <USmallFlat/static_flat_multiset.hpp>
#include <USmallFlat/small_flat_multiset.hpp>
#include <USmallFlat/flat_multiset.hpp>
#include <USmallFlat/pmr/flat_multiset.hpp>

using doctest::test_suite;
using namespace Ubpa;
#include <string>
#include <memory_resource>

TEST_CASE("static flat set" * test_suite("all")) {
  static_assert(sizeof(static_flat_set<int, 5>) == sizeof(static_vector<int, 5>));
  {
    static_flat_set<int, 5> v;
    REQUIRE(v.empty());
  }
  {
    static_flat_set<int, 5> v;
    v.insert(1);
    v.insert(4);
    v.insert(1);
    v.insert(3);
    v.insert(2);
    REQUIRE(v == static_flat_set<int, 5>{1, 2, 3, 4});
  }
  {
    static_flat_set<int, 5> v{1,2,3,1,2};
    REQUIRE(v == static_flat_set<int, 5>{1, 2, 3});
  }
  {
    static_flat_set<int, 5> v{1,2,3};
    v.insert(v.begin(), 5);
    REQUIRE(v == static_flat_set<int, 5>{1, 2, 3, 5});
    v.insert(v.begin(), 5);
    REQUIRE(v == static_flat_set<int, 5>{1, 2, 3, 5});
    v.insert(v.begin() + 3, 4);
    REQUIRE(v == static_flat_set<int, 5>{1, 2, 3, 4, 5});
  }
  {
    static_flat_set<int, 5> v{1,2,3};
    v.erase(2);
    REQUIRE(v == static_flat_set<int, 5>{1, 3});
  }
  {
    static_flat_set<int, 5> v{1,2,4};
    auto iter = v.lower_bound(3);
    REQUIRE(iter == v.begin() + 2);
    v.insert(iter, 3);
    REQUIRE(v == static_flat_set<int, 5>{1, 2, 3, 4});
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
    static_vector<int, 5> s{1,2,4};
    static_flat_set<int, 5> v{ s };
    auto iter = v.lower_bound(3);
    v.insert(iter, 3);
    REQUIRE(v == static_flat_set<int, 5>{1, 2, 3, 4});
  }
  //--
  {
    static_flat_multiset<int, 5> v{ 1,1,2,3 };
    v.insert(2);
    REQUIRE(v == static_flat_multiset<int, 5>{1, 1, 2, 2, 3});
  }
}
