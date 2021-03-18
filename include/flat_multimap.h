#pragma once

#include "details/flat_base_multimap.h"

namespace Ubpa {
    // require
    // - Vector<std::pair<const Key, T>>::iterator <=> Vector<std::pair<Key, T>>::iterator
    // - Vector<std::pair<const Key, T>>::const_iterator <=> Vector<std::pair<Key, T>>::const_iterator
    template <typename Key, typename T, template<typename>class Vector, typename Compare>
    class flat_multimap : public details::flat_base_multimap<flat_multimap<Key, T, Vector, Compare>, true, Key, T, Vector, Compare> {
        using mybase = details::flat_base_multimap<flat_multimap<Key, T, Vector, Compare>, true, Key, T, Vector, Compare>;
    public:
        using mybase::mybase;
        using mybase::operator=;
    };
}
