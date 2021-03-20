#pragma once

#include "../basic_flat_multiset.hpp"

#include <vector>

namespace Ubpa::pmr {
    template<typename Key, typename Compare = std::less<Key>>
    class flat_multiset : public basic_flat_multiset<std::pmr::vector, Key, Compare> {
        using mybase = basic_flat_multiset<std::pmr::vector, Key, Compare>;
    public:
        using mybase::mybase;
        using typename mybase::value_type;

        flat_multiset(std::initializer_list<Key> ilist, const Compare& comp = Compare())
            : mybase(ilist, comp) {}
    };
}
