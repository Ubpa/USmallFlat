#include "flat_map.h"

#include <vector>

namespace Ubpa::details {
    template<typename Allocator>
    struct vector_flat_map_helper {
        template<typename T>
        using Ttype = std::vector<T, Allocator>;
    };
}

namespace Ubpa {
    template<typename Key, typename Compare = std::less<Key>, typename Allocator = std::allocator<Key>>
    using vector_flat_map = flat_map<Key,
        details::vector_flat_map_helper<Allocator>::template Ttype,
        Compare>;
}
