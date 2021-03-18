#include "basic_flat_map.hpp"

#include "details/small_vector_bind.hpp"

namespace Ubpa {
    template<typename Key, typename T, std::size_t N = 16, typename Compare = std::less<Key>, typename Allocator = std::allocator<std::pair<Key, T>>>
    using small_flat_map = basic_flat_map<details::small_vector_bind<N, Allocator>::template Ttype,
        Key, T, Compare>;
}
