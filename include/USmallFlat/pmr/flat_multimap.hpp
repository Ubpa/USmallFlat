#pragma once

#include "../basic_flat_multimap.hpp"

#include <vector>

namespace Ubpa::pmr {
    template<typename Key, typename T, typename Compare = std::less<Key>>
    class flat_multimap : public basic_flat_multimap<std::pmr::vector, Key, T, Compare> {
        using mybase = basic_flat_multimap<std::pmr::vector, Key, T, Compare>;
    public:
        using mybase::mybase;
        using typename mybase::value_type;

        flat_multimap(std::initializer_list<value_type> ilist, const Compare& comp = Compare())
            : mybase(ilist, comp) {}
    };
}
