#pragma once

#include "details/flat_base_multiset.hpp"

namespace Ubpa {
    template <template<typename>class Vector, typename Key, typename Compare = std::less<Key>>
    class basic_flat_multiset : public details::flat_base_multiset<basic_flat_multiset<Vector, Key, Compare>, true, Vector, Key, Compare> {
        using mybase = details::flat_base_multiset<basic_flat_multiset<Vector, Key, Compare>, true, Vector, Key, Compare>;
    public:
        using mybase::mybase;
    };
}
