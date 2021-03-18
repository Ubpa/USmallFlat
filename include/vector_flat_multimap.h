#include "flat_multimap.h"

#include <vector>

namespace Ubpa::details {
    template<typename Allocator>
    struct vector_flat_multimap_helper {
        template<typename T>
        using Ttype = std::vector<T, Allocator>;
    };
}

namespace Ubpa {
    template<typename Key, typename T, typename Compare = std::less<Key>, typename Allocator = std::allocator<Key>>
    using vector_flat_multimap = flat_multimap<Key, T,
        details::vector_flat_multimap_helper<Allocator>::template Ttype,
        Compare>;
}
