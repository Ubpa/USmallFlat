#pragma once

#include "basic_flat_multimap.hpp"

#include "details/vector_bind.hpp"

namespace Ubpa {
    template<typename Key, typename T, typename Compare = std::less<Key>, template<typename>class TAllocator = std::allocator>
    using flat_multimap = basic_flat_multimap<details::Tvector_bind<TAllocator>::template Ttype,
        Key, T, Compare>;
}
