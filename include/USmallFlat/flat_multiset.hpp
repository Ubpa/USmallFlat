#pragma once

#include "basic_flat_multiset.hpp"

#include "details/vector_bind.hpp"

namespace Ubpa {
    template<typename Key, typename Compare = std::less<Key>, template<typename>class TAllocator = std::allocator>
    class flat_multiset : public basic_flat_multiset<details::Tvector_bind<TAllocator>::template Ttype, Key, Compare> {
        using mybase = basic_flat_multiset<details::Tvector_bind<TAllocator>::template Ttype, Key, Compare>;
    public:
        using mybase::mybase;

        flat_multiset(std::initializer_list<Key> ilist, const Compare& comp = Compare())
            : mybase(ilist, comp) {}
    };
}
