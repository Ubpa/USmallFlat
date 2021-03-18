#pragma once

#include "details/flat_base_multiset.h"

namespace Ubpa {
    template <typename T, template<typename>class Vector, typename Compare = std::less<T>>
    class flat_multiset : public details::flat_base_multiset<flat_multiset<T, Vector, Compare>, true, T, Vector, Compare> {
        using mybase = details::flat_base_multiset<flat_multiset<T, Vector, Compare>, true, T, Vector, Compare>;
    public:
        using mybase::mybase;
        using mybase::operator=;
    };
}
