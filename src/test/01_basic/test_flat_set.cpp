#include "doctest.hpp"
#include <flat_set.h>
#include <small_vector.h>
using doctest::test_suite;
using namespace Ubpa;
#include <string>
#include <memory_resource>

template<typename Key, std::size_t N, typename Compare = std::less<Key>>
using fixed_flat_set = flat_set<fixed_vector<Key, N>, Compare>;
template<typename Key, std::size_t N, typename Compare = std::less<Key>>
using small_flat_set = flat_set<small_vector<Key, N>, Compare>;
template<typename Key, typename Compare = std::less<Key>, typename Alloc = typename std::vector<Key>::allocator_type>
using std_vector_flat_set = flat_set<std::vector<Key, Alloc>, Compare>;
template<typename Key, typename Compare = std::less<Key>>
using std_pmr_vector_flat_set = flat_set<std::pmr::vector<Key>, Compare>;

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
    std_pmr_vector_flat_set<int> v;
    REQUIRE(v.empty());
  }
  {
    std_pmr_vector_flat_set<int> v;
    v.insert(1);
    v.insert(2);
    v.insert(1);
    REQUIRE(v == std_pmr_vector_flat_set<int>{1, 2});
  }
  {
    std_pmr_vector_flat_set<int> v{1,2,3,1,2};
    REQUIRE(v == std_pmr_vector_flat_set<int>{1, 2, 3});
  }
  {
    std_pmr_vector_flat_set<int> v{1,2,3};
    v.insert(v.begin(), 5);
    REQUIRE(v == std_pmr_vector_flat_set<int>{1, 2, 3, 5});
    v.insert(v.begin(), 5);
    REQUIRE(v == std_pmr_vector_flat_set<int>{1, 2, 3, 5});
    v.insert(v.begin() + 3, 4);
    REQUIRE(v == std_pmr_vector_flat_set<int>{1, 2, 3, 4, 5});
  }
  {
    std_pmr_vector_flat_set<int> v{1,2,3};
    v.erase(2);
    REQUIRE(v == std_pmr_vector_flat_set<int>{1, 3});
  }
  {
    std_pmr_vector_flat_set<int> v{1,2,4};
    auto iter = v.lower_bound(3);
    REQUIRE(iter == v.begin() + 2);
    v.insert(iter, 3);
    REQUIRE(v == std_pmr_vector_flat_set<int>{1, 2, 3, 4});
  }
}
