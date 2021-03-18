#pragma once

#include "../fixed_vector.hpp"

namespace Ubpa::details {
    template<std::size_t N>
    struct fixed_vector_bind {
        template<typename T>
        using Ttype = fixed_vector<T, N>;
    };
}
