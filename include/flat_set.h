#pragma once

#include "basic_flat_set.h"

#include "details/vector_bind.h"

namespace Ubpa {
    template<typename Key, typename Compare = std::less<Key>, typename Allocator = std::allocator<Key>>
    using flat_set = basic_flat_set<details::vector_bind<Allocator>::template Ttype,
        Key, Compare>;
}
