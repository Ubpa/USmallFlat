#pragma once

#include "../small_vector.hpp"

namespace Ubpa::pmr::details {
    template<std::size_t N>
    struct small_vector_bind {
        template<typename T>
        using Ttype = Ubpa::pmr::small_vector<T, N>;
    }
}
