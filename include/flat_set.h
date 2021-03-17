#pragma once

#include "details/flat_base_multiset.h"

#include <cassert>
#include <algorithm>
#include <concepts>
#include <type_traits>

namespace Ubpa {
    template <typename T, template<typename>class Vector, typename Compare = std::less<T>>
    class flat_set : public details::flat_base_multiset<flat_set<T, Vector, Compare>, false, T, Vector, Compare> {
        using mybase = details::flat_base_multiset<flat_set<T, Vector, Compare>, false, T, Vector, Compare>;
    public:
        using container_type = typename mybase::container_type;
        using key_type = typename mybase::key_type;
        using value_type = typename mybase::value_type;
        using size_type = typename mybase::size_type;
        using difference_type = typename mybase::difference_type;
        using key_compare = typename mybase::key_compare;
        using value_compare = typename mybase::value_compare;
        using iterator = typename mybase::iterator;
        using const_iterator = typename mybase::const_iterator;
        using reverse_iterator = typename mybase::reverse_iterator;
        using const_reverse_iterator = typename mybase::const_reverse_iterator;

        using mybase::mybase;
        using mybase::operator=;
    };
}
