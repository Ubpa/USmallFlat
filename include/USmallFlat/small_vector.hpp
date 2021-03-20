#pragma once

#include "basic_small_vector.hpp"

#include "details/vector_bind.hpp"

namespace Ubpa {
    template<typename T, std::size_t N = 16, typename Allocator = std::allocator<T>>
    class small_vector : public basic_small_vector<details::vector_bind<Allocator>::template Ttype, T, N> {
        using mybase = basic_small_vector<details::vector_bind<Allocator>::template Ttype, T, N>;
    public:
        using mybase::mybase;
        small_vector(std::initializer_list<T> ilist) : mybase(ilist) {}
    };
}
