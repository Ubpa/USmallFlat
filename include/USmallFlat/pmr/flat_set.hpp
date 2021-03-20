#pragma once

#include "../basic_flat_set.hpp"

#include <vector>

namespace Ubpa::pmr {
    template<typename Key, typename Compare = std::less<Key>>
    class flat_set : public basic_flat_set<std::pmr::vector, Key, Compare> {
        using mybase = basic_flat_set<std::pmr::vector, Key, Compare>;
    public:
        using mybase::mybase;
        using typename mybase::value_type;

        flat_set(std::initializer_list<Key> ilist, const Compare& comp = Compare())
            : mybase(ilist, comp) {}
    };
}
