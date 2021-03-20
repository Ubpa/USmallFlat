#pragma once

#include "../basic_small_vector.hpp"

#include <vector>

namespace Ubpa::pmr {
    template<typename T, std::size_t N = 16>
    class small_vector : public basic_small_vector<std::pmr::vector, T, N> {
        using mybase = basic_small_vector<std::pmr::vector, T, N>;
    public:
        using mybase::mybase;
        small_vector(std::initializer_list<T> ilist) : mybase(ilist) {}
    };
}
