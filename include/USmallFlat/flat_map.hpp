#pragma once

#include "basic_flat_map.hpp"

#include "details/vector_bind.hpp"

namespace Ubpa {
    template<typename Key, typename T, typename Compare = std::less<Key>, template<typename>class TAllocator = std::allocator>
    class flat_map : public basic_flat_map<details::Tvector_bind<TAllocator>::template Ttype, Key, T, Compare> {
        using mybase = basic_flat_map<details::Tvector_bind<TAllocator>::template Ttype, Key, T, Compare>;
    public:
        using mybase::mybase;
        using typename mybase::value_type;

        flat_map(std::initializer_list<value_type> ilist, const Compare& comp = Compare())
            : mybase(ilist, comp) {}
    };
}
