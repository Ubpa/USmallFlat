#pragma once

#include "basic_flat_multimap.hpp"

#include "details/vector_bind.hpp"

namespace Ubpa {
    template<typename Key, typename T, typename Compare = std::less<Key>, template<typename>class TAllocator = std::allocator>
    class flat_multimap : public basic_flat_multimap<details::Tvector_bind<TAllocator>::template Ttype, Key, T, Compare> {
        using mybase = basic_flat_multimap<details::Tvector_bind<TAllocator>::template Ttype, Key, T, Compare>;
    public:
        using mybase::mybase;
        using typename mybase::value_type;

        flat_multimap(std::initializer_list<value_type> ilist, const Compare& comp = Compare())
            : mybase(ilist, comp) {}
    };
}
