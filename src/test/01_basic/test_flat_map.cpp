#include "doctest.hpp"
#include <USmallFlat/flat_map.hpp>
#include <USmallFlat/static_flat_set.hpp>
#include <USmallFlat/static_flat_map.hpp>
#include <USmallFlat/flat_map.hpp>
#include <USmallFlat/small_vector.hpp>
using doctest::test_suite;
using namespace Ubpa;
#include <string>
#include <memory_resource>
#include <map>

TEST_CASE("static flat map" * test_suite("all")) {
  static_assert(sizeof(static_flat_map<int, int, 5>) == sizeof(static_flat_set<std::pair<int, int>, 5>));
  {
    static_flat_map<int, int, 5> m{{0,1}, {1,2}};
    m.try_emplace(3, 4);
    static_flat_map<int, int, 5> p{ {0, 1}, { 1,2 }, { 3,4 } };
    REQUIRE(m == p);
  }
  {
    static_flat_map<int, int, 5> m{{0,1}, {2,3}};
    m.try_emplace(m.begin() + 1, 1, 4);
    static_flat_map<int, int, 5> p{ {0, 1}, { 1,4 }, { 2,3 } };
    REQUIRE(m == p);
  }
  {
    static_flat_map<int, int, 5> m{ {0,1}, {1,2} };
    m[1] = 3;
    static_flat_map<int, int, 5> p{ {0,1}, {1,3} };
    REQUIRE(m == p);
  }
  {
    static_flat_map<int, int, 5> m{ {0,1}, {1,2} };
    m[1] = 3;
    static_flat_map<int, int, 5> p{ {0,1}, {1,3} };
    REQUIRE(m == p);
  }
  {
    flat_map<int, int> m{ {0,1}, {1,2} };
    m[1] = 3;
    flat_map<int, int> p{ {0,1}, {1,3} };
    REQUIRE(m == p);
  }
}
