#pragma once

#include "../small_vector.hpp"

namespace Ubpa::details {
    template<std::size_t N, template<typename>class TAllocator>
    struct Tsmall_vector_bind {
        template<typename T>
        using Ttype = small_vector<T, N, TAllocator<T>>;
    };

    template<std::size_t N, typename Allocator>
    struct small_vector_bind {
        template<typename T>
        using Ttype = small_vector<T, N, Allocator>;
    };
}
