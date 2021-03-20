#pragma once

#include "basic_flat_multimap.hpp"

#include "details/static_vector_bind.hpp"

namespace Ubpa {
    template<typename Key, typename T, std::size_t N = 16, typename Compare = std::less<Key>>
    class static_flat_multimap : public basic_flat_multimap<details::static_vector_bind<N>::template Ttype, Key, T, Compare> {
        using mybase = basic_flat_multimap<details::static_vector_bind<N>::template Ttype, Key, T, Compare>;
    public:
        using mybase::mybase;
        using typename mybase::value_type;

        static_flat_multimap(std::initializer_list<value_type> ilist, const Compare& comp = Compare())
            : mybase(ilist, comp) {}
    };
}
