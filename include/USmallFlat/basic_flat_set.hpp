#pragma once

#include "details/flat_base_multiset.hpp"

namespace Ubpa {
    template <template<typename>class Vector, typename Key, typename Compare = std::less<Key>>
    class basic_flat_set : public details::flat_base_multiset<basic_flat_set<Vector, Key, Compare>, false, Vector, Key, Compare> {
        using mybase = details::flat_base_multiset<basic_flat_set<Vector, Key, Compare>, false, Vector, Key, Compare>;
    public:
        using mybase::mybase;
    };
}
