#pragma once

#include "basic_flat_set.hpp"

#include "details/static_vector_bind.hpp"

namespace Ubpa {
    template<typename Key, std::size_t N = 16, typename Compare = std::less<Key>>
    class static_flat_set : public basic_flat_set<details::static_vector_bind<N>::template Ttype, Key, Compare> {
        using mybase = basic_flat_set<details::static_vector_bind<N>::template Ttype, Key, Compare>;
    public:
        using mybase::mybase;
        using typename mybase::value_type;

        static_flat_set(std::initializer_list<value_type> ilist, const Compare& comp = Compare())
            : mybase(ilist, comp) {}
    };
}
