#pragma once

#include "basic_flat_multiset.h"

#include "details/vector_bind.h"

namespace Ubpa {
    template<typename Key, typename Compare = std::less<Key>, typename Allocator = std::allocator<Key>>
    using flat_multiset = basic_flat_multiset<details::vector_bind<Allocator>::template Ttype,
        Key, Compare>;
}
