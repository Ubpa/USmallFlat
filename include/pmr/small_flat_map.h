#pragma once

#include "../basic_flat_map.h"

#include "details/small_vector_bind.h"

namespace Ubpa::pmr {
    template<typename Key, typename T, std::size_t N = 16, typename Compare = std::less<Key>>
    using small_flat_map = basic_flat_map<Ubpa::pmr::details::small_vector_bind<N>::template Ttype,
        Key, T, Compare>;
}
