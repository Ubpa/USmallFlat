#pragma once

#include "basic_flat_multimap.h"

#include "details/vector_bind.h"

namespace Ubpa {
    template<typename Key, typename T, typename Compare = std::less<Key>, template<typename>class TAllocator = std::allocator>
    using flat_multimap = basic_flat_multimap<details::Tvector_bind<TAllocator>::template Ttype,
        Key, T, Compare>;
}
