#include "flat_set.h"

#include "small_vector.h"

namespace Ubpa::details {
    template<std::size_t N, typename Allocator>
    struct small_flat_set_helper {
        template<typename T>
        using Ttype = small_vector<T, N, Allocator>;
    };
}

namespace Ubpa {
    template<typename Key, std::size_t N = 16, typename Compare = std::less<Key>, typename Allocator = std::allocator<Key>>
    using small_flat_set = flat_set<Key,
        details::small_flat_set_helper<N, Allocator>::template Ttype,
        Compare>;
}
