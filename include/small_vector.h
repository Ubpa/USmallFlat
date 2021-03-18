#pragma once

#include "basic_small_vector.h"

#include "details/vector_bind.h"

namespace Ubpa {
    template<typename T, std::size_t N = 16, typename Allocator = std::allocator<T>>
    using small_vector = basic_small_vector<details::vector_bind<Allocator>::template Ttype,
        T, N>;
}
