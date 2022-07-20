#include "doctest.h"
#include <USmallFlat/small_vector.hpp>
using doctest::test_suite;
using namespace Ubpa;
#include <string>

TEST_CASE("Default construct a small vector of ints" * test_suite("constructors")) {
  small_vector<int, 5> vec;
  REQUIRE(vec.size() == 0);
}

TEST_CASE("Class template argument deduction" * test_suite("constructors")) {
    small_vector vec{ 1,2,3,4 };
    REQUIRE(vec == small_vector<int>{1, 2, 3, 4});
}

TEST_CASE("Fill construct a small vector of floats" * test_suite("constructors")) {
  small_vector<float, 10> vec(20, 1.0);
  REQUIRE(vec.size() == 20);
  std::size_t count = 0;
  for (size_t i = 0; i < vec.size(); i++, count++) {
    REQUIRE(vec[i] == 1.0);
  }
  REQUIRE(count == vec.size());
}

TEST_CASE("Copy construct a small vector of strings" * test_suite("constructors")) {
  small_vector<std::string, 5> foo {"a", "b", "c", "d", "e", "f"};
  small_vector<std::string, 5> bar(foo);
  REQUIRE(bar.size() == 6);
  REQUIRE(bar[0] == "a");
  REQUIRE(bar[1] == "b");
  REQUIRE(bar[2] == "c");
  REQUIRE(bar[3] == "d");
  REQUIRE(bar[4] == "e");
  REQUIRE(bar[5] == "f");
}

TEST_CASE("Copy construct a small vector by iterator" * test_suite("constructors")) {
  std::vector<int> ints{ 1,2,3,4,5,6 };
  small_vector<int, 5> copied{ ints.begin(), ints.end() };
  REQUIRE(copied == small_vector<int, 5>{1, 2, 3, 4, 5, 6});
}

TEST_CASE("Move construct a small vector of chars" * test_suite("constructors")) {
  small_vector<char, 5> bar(std::move(small_vector<char, 5>{'a', 'b', 'c'}));
  REQUIRE(bar.size() == 3);
  REQUIRE(bar[0] == 'a');
  REQUIRE(bar[1] == 'b');
  REQUIRE(bar[2] == 'c');
}

TEST_CASE("Construct a small vector of bools from initializer list" * test_suite("constructors")) {
  small_vector<uint8_t, 5> foo{1, 0, 1, 0, 1, 1, 1, 1};
  REQUIRE(foo.size() == 8);
  REQUIRE(foo[0] == static_cast<uint8_t>(true));
  REQUIRE(foo[1] == static_cast<uint8_t>(false));
  REQUIRE(foo[2] == static_cast<uint8_t>(true));
  REQUIRE(foo[3] == static_cast<uint8_t>(false));
  REQUIRE(foo[4] == static_cast<uint8_t>(true));
  REQUIRE(foo[5] == static_cast<uint8_t>(true));
  REQUIRE(foo[6] == static_cast<uint8_t>(true));
  REQUIRE(foo[7] == static_cast<uint8_t>(true));
}
