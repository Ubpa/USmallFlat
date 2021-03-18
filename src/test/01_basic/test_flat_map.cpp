#include "doctest.hpp"
#include <flat_map.h>
#include <fixed_flat_set.h>
#include <small_vector.h>
using doctest::test_suite;
using namespace Ubpa;
#include <string>
#include <memory_resource>
#include <map>
namespace Ubpa::details {
    template<std::size_t N>
    struct fixed_flat_map_helper {
        template<typename T>
        using Ttype = fixed_vector<T, N>;
    };
}
template<typename Key, typename T, std::size_t N, typename Compare = std::less<Key>>
using fixed_flat_map = flat_map<Key, T, details::fixed_flat_map_helper<N>::template Ttype, Compare>;

TEST_CASE("fixed flat map" * test_suite("all")) {
  static_assert(sizeof(fixed_flat_map<int, int, 5>) == sizeof(fixed_flat_set<std::pair<int, int>, 5>));
  {
    fixed_flat_map<int, int, 5> m{{0,1}, {1,2}};
    m.try_emplace(3, 4);
    fixed_flat_map<int, int, 5> p{ {0, 1}, { 1,2 }, { 3,4 } };
    REQUIRE(m == p);
  }
  {
    fixed_flat_map<int, int, 5> m{{0,1}, {2,3}};
    m.try_emplace(m.begin() + 1, 1, 4);
    fixed_flat_map<int, int, 5> p{ {0, 1}, { 1,4 }, { 2,3 } };
    REQUIRE(m == p);
  }
  {
    fixed_flat_map<int, int, 5> m{ {0,1}, {1,2} };
    m[1] = 3;
    fixed_flat_map<int, int, 5> p{ {0,1}, {1,3} };
    REQUIRE(m == p);
  }
  {
    fixed_flat_map<int, int, 5> m{ {0,1}, {1,2} };
    m[1] = 3;
    fixed_flat_map<int, int, 5> p{ {0,1}, {1,3} };
    REQUIRE(m == p);
  }
}
