#pragma once

#include "details/flat_base_multiset.hpp"

namespace Ubpa {
    template <template<typename>class Vector, typename T, typename Compare = std::less<T>>
    class basic_flat_multiset : public details::flat_base_multiset<basic_flat_multiset<Vector, T, Compare>, true, Vector, T, Compare> {
        using mybase = details::flat_base_multiset<basic_flat_multiset<Vector, T, Compare>, true, Vector, T, Compare>;
    public:
        using mybase::mybase;
        using mybase::operator=;
    };
}
