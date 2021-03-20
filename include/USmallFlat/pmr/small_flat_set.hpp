#pragma once

#include "../basic_flat_set.hpp"

#include "details/small_vector_bind.hpp"

namespace Ubpa::pmr {
    template<typename Key, std::size_t N = 16, typename Compare = std::less<Key>>
    class small_flat_set : public basic_flat_set<details::small_vector_bind<N>::template Ttype, Key, Compare> {
        using mybase = basic_flat_set<details::small_vector_bind<N>::template Ttype, Key, Compare>;
    public:
        using mybase::mybase;
        using typename mybase::value_type;

        small_flat_set(std::initializer_list<value_type> ilist, const Compare& comp = Compare())
            : mybase(ilist, comp) {}
    };
}
