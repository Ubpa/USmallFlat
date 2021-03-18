#pragma once

#include "../basic_flat_set.hpp"

#include "details/small_vector_bind.hpp"

namespace Ubpa::pmr {
    template<typename Key, std::size_t N = 16, typename Compare = std::less<Key>>
    using small_flat_set = basic_flat_set<Ubpa::pmr::details::small_vector_bind<N>::template Ttype,
        Key, Compare>;
}
