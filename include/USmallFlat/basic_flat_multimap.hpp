#pragma once

#include "details/flat_base_multimap.hpp"

namespace Ubpa {
    // require
    // - Vector<std::pair<const Key, T>>::iterator <=> Vector<std::pair<Key, T>>::iterator
    // - Vector<std::pair<const Key, T>>::const_iterator <=> Vector<std::pair<Key, T>>::const_iterator
    template <template<typename>class Vector, typename Key, typename T, typename Compare>
    class basic_flat_multimap : public details::flat_base_multimap<basic_flat_multimap<Vector, Key, T, Compare>, true, Vector, Key, T, Compare> {
        using mybase = details::flat_base_multimap<basic_flat_multimap<Vector, Key, T, Compare>, true, Vector, Key, T, Compare>;
    public:
        using mybase::mybase;
    };
}
