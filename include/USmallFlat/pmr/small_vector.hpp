#pragma once

#include "../basic_small_vector.hpp"

#include <vector>

namespace Ubpa::pmr {
    template<typename T, std::size_t N = 16>
    using small_vector = basic_small_vector<std::pmr::vector, T, N>;
}
