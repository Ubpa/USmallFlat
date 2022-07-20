#include "doctest.h"
#include <USmallFlat/static_vector.hpp>
using doctest::test_suite;
using namespace Ubpa;
#include <string>

TEST_CASE("static vector" * test_suite("all")) {
  //////////////////////
  // Member functions //
  //////////////////////
  {
    static_vector<int, 5> v;
    REQUIRE(v.empty());
  }
  {
    static_vector<int, 5> v(2);
    REQUIRE(v.size() == 2);
    REQUIRE(v[0] == 0);
    REQUIRE(v[1] == 0);
  }
  {
    static_vector<int, 5> v(2,3);
    REQUIRE(v.size() == 2);
    REQUIRE(v[0] == 3);
    REQUIRE(v[1] == 3);
  }
  {
    static_vector<int, 5> v(2, 3);
    REQUIRE(v.size() == 2);
    REQUIRE(v[0] == 3);
    REQUIRE(v[1] == 3);
  }
  {
    static_vector<int, 5> v(2, 3);
    static_vector<int, 5> w(v);
    REQUIRE(w.size() == 2);
    REQUIRE(w[0] == 3);
    REQUIRE(w[1] == 3);
  }
  {
    static_vector<int, 5> v(2, 3);
    static_vector<int, 5> w(std::move(v));
    REQUIRE(w.size() == 2);
    REQUIRE(w[0] == 3);
    REQUIRE(w[1] == 3);
  }
  {
    static_vector<int, 5> v(2, 3);
    static_vector<int, 5> w(std::move(v));
    REQUIRE(v.empty());
    REQUIRE(w.size() == 2);
    REQUIRE(w[0] == 3);
    REQUIRE(w[1] == 3);
  }
  {
    static_vector<int, 5> v{1,2,3};
    REQUIRE(v.size() == 3);
    REQUIRE(v[0] == 1);
    REQUIRE(v[1] == 2);
    REQUIRE(v[2] == 3);
  }
  {
    static_vector<int, 5> v{1,2,3};
    static_vector<int, 5> w{4,3,2,1};
    v = w;
    REQUIRE(v.size() == 4);
    REQUIRE(v[0] == 4);
    REQUIRE(v[1] == 3);
    REQUIRE(v[2] == 2);
    REQUIRE(v[3] == 1);
  }
  {
    static_vector<int, 5> v{1,2,3};
    static_vector<int, 5> w{4,3,2,1};
    v = std::move(w);
    REQUIRE(w.empty());
    REQUIRE(v.size() == 4);
    REQUIRE(v[0] == 4);
    REQUIRE(v[1] == 3);
    REQUIRE(v[2] == 2);
    REQUIRE(v[3] == 1);
  }
  {
    static_vector<int, 5> v{1,2,3};
    v = { 4,3,2,1 };
    REQUIRE(v.size() == 4);
    REQUIRE(v[0] == 4);
    REQUIRE(v[1] == 3);
    REQUIRE(v[2] == 2);
    REQUIRE(v[3] == 1);
  }
  {
    static_vector<int, 5> v{1,2,3};
    REQUIRE(v.at(2) == 3);
    REQUIRE_THROWS(v.at(3));
  }
  {
    static_vector<int, 5> v{1,2,3};
    v.clear();
    REQUIRE(v.empty());
  }
  {
    static_vector<int, 5> v{1,2,3,4};
    v.push_back(5);
    REQUIRE(v[4] == 5);
    v.pop_back();
    REQUIRE(v.size() == 4);
    v.emplace_back(5);
    REQUIRE(v[4] == 5);
    v.resize(2);
    REQUIRE(v.size() == 2);
    v.resize(4, 5);
    REQUIRE(v.size() == 4);
    REQUIRE(v[2] == 5);
    REQUIRE(v[3] == 5);
  }
  {
    static_vector<int, 5> v{ 1,2 };
    static_vector<int, 5> w{ 3,2,1 };
    std::swap(v, w);
    REQUIRE(v.size() == 3);
    REQUIRE(v[0] == 3);
    REQUIRE(v[1] == 2);
    REQUIRE(v[2] == 1);
    REQUIRE(w.size() == 2);
    REQUIRE(w[0] == 1);
    REQUIRE(w[1] == 2);
  }
  {
    static_vector<int, 5> v{1,2};
    v.insert(v.begin() + 1, 4);
    v.insert(v.begin() + 1, 2, 5);
    REQUIRE(v.size() == 5);
    REQUIRE(v[0] == 1);
    REQUIRE(v[1] == 5);
    REQUIRE(v[2] == 5);
    REQUIRE(v[3] == 4);
    REQUIRE(v[4] == 2);
  }
  {
    static_vector<int, 5> v{1,2};
    static_vector<int, 5> w{5,5,4};
    v.insert(v.begin() + 1, w.begin(), w.end());
    REQUIRE(v.size() == 5);
    REQUIRE(v[0] == 1);
    REQUIRE(v[1] == 5);
    REQUIRE(v[2] == 5);
    REQUIRE(v[3] == 4);
    REQUIRE(v[4] == 2);
  }
  {
    static_vector<int, 5> v;
    v.assign(3, 2);
    REQUIRE(v.size() == 3);
    REQUIRE(v[0] == 2);
    REQUIRE(v[1] == 2);
    REQUIRE(v[2] == 2);
    static_vector<int, 5> w{ 1,2 };
    v.assign(w.begin(), w.end());
    REQUIRE(v.size() == 2);
    REQUIRE(v[0] == 1);
    REQUIRE(v[1] == 2);
  }
  {
    static_vector<int, 5> v{1,2,3,4,5};
    v.erase(v.begin() + 1, v.begin() + 3);
    REQUIRE(v.size() == 3);
    REQUIRE(v[0] == 1);
    REQUIRE(v[1] == 4);
    REQUIRE(v[2] == 5);
  }
  {
    static_vector<int, 5> v{1,2,3,4,5};
    static_vector<int, 5> w{v.rbegin(), v.rend()};
    REQUIRE(w == static_vector<int, 5>{ 5,4,3,2,1 });
  }
}
