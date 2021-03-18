#pragma once

#include "../basic_flat_multiset.h"

#include "details/small_vector_bind.h"

namespace Ubpa::pmr {
    template<typename Key, std::size_t N = 16, typename Compare = std::less<Key>>
    using small_flat_multiset = basic_flat_multiset<Ubpa::pmr::details::small_vector_bind<N>::template Ttype,
        Key, Compare>;
}
