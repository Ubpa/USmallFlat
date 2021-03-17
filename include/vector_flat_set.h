#include "flat_set.h"

#include <vector>

namespace Ubpa::details {
    template<typename Allocator>
    struct vector_flat_set_helper {
        template<typename T>
        using Ttype = std::vector<T, Allocator>;
    };
}

namespace Ubpa {
    template<typename Key, typename Compare = std::less<Key>, typename Allocator = std::allocator<Key>>
    using vector_flat_set = flat_set<Key,
        details::vector_flat_set_helper<Allocator>::template Ttype,
        Compare>;
}
