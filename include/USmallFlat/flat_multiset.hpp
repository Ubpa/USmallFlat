#pragma once

#include "basic_flat_multiset.hpp"

#include "details/vector_bind.hpp"

namespace Ubpa {
    template<typename Key, typename Compare = std::less<Key>, typename Allocator = std::allocator<Key>>
    using flat_multiset = basic_flat_multiset<details::vector_bind<Allocator>::template Ttype,
        Key, Compare>;
}
