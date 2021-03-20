#pragma once

#include "../static_vector.hpp"

namespace Ubpa::details {
    template<std::size_t N>
    struct static_vector_bind {
        template<typename T>
        using Ttype = static_vector<T, N>;
    };
}
