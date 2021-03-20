#pragma once

#include "basic_flat_multiset.hpp"

#include "details/small_vector_bind.hpp"

namespace Ubpa {
    template<typename Key, std::size_t N = 16, typename Compare = std::less<Key>, template<typename>class TAllocator = std::allocator>
    class small_flat_multiset : public basic_flat_multiset<details::Tsmall_vector_bind<N, TAllocator>::template Ttype, Key, Compare> {
        using mybase = basic_flat_multiset<details::Tsmall_vector_bind<N, TAllocator>::template Ttype, Key, Compare>;
    public:
        using mybase::mybase;
        using typename mybase::value_type;

        small_flat_multiset(std::initializer_list<value_type> ilist, const Compare& comp = Compare())
            : mybase(ilist, comp) {}
    };
}
