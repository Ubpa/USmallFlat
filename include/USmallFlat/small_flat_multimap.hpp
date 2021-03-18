#include "basic_flat_multimap.hpp"

#include "details/small_vector_bind.hpp"

namespace Ubpa {
    template<typename Key, typename T, std::size_t N = 16, typename Compare = std::less<Key>, template<typename>class TAllocator = std::allocator>
    using small_flat_multimap = basic_flat_multimap<details::Tsmall_vector_bind<N, TAllocator>::template Ttype,
        Key, T, Compare>;
}
