#pragma once

#include "basic_small_vector.hpp"

#include "details/vector_bind.hpp"

namespace Ubpa {
    template<typename T, std::size_t N = 16, typename Allocator = std::allocator<T>>
    using small_vector = basic_small_vector<details::vector_bind<Allocator>::template Ttype,
        T, N>;
}
