#include "doctest.h"
#include <USmallFlat/small_vector.hpp>
using doctest::test_suite;
using namespace Ubpa;
#include <string>
#include <set>

TEST_CASE("Sizing/Resizing in small vector of ints" * test_suite("sizing")) {
  small_vector<int, 5> a{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  REQUIRE_FALSE(a.empty());
  REQUIRE(a.size() == 10);

  a.resize(6);
  REQUIRE(a.size() == 6);

  a.resize(20);
  REQUIRE(a.size() == 20);
  std::size_t i = 0; 
  for (auto j = 6; j < a.size(); j++) {
    REQUIRE(a[j] == 0);
    i += 1;
  }
  REQUIRE(i == 14);

  {
    small_vector<int, 5> v{1,2,3};
    v.insert(v.begin(), 5);
    v.insert(v.begin(), { 5,4,3 });
    REQUIRE(v == small_vector<int, 5>{5, 4, 3, 5, 1, 2, 3});
  }
  {
    small_vector<int, 5> v{1,2,3};
    std::set s{ 5,4,3 };
    v.insert(v.begin(), s.begin(), s.end());
    REQUIRE(v == small_vector<int, 5>{3, 4, 5, 1, 2, 3});
  }
  {
    small_vector<int, 5> v;
    v.assign({ 5,4,3 });
    REQUIRE(v == small_vector<int, 5>{5, 4, 3});
  }
  {
    small_vector<int, 5> v;
    std::set s{ 5,4,3 };
    v.assign(s.begin(), s.end());
    REQUIRE(v == small_vector<int, 5>{3, 4, 5});
  }
  {
    small_vector<int, 5> v{1,2,3};
    v.erase(v.begin(), v.begin() + 1);
    REQUIRE(v == small_vector<int, 5>{2,3});
  }
}
