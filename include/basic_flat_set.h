#pragma once

#include "details/flat_base_multiset.h"

namespace Ubpa {
    template <template<typename>class Vector, typename T, typename Compare = std::less<T>>
    class basic_flat_set : public details::flat_base_multiset<basic_flat_set<Vector, T, Compare>, false, Vector, T, Compare> {
        using mybase = details::flat_base_multiset<basic_flat_set<Vector, T, Compare>, false, Vector, T, Compare>;
    public:
        using mybase::mybase;
        using mybase::operator=;
    };
}
