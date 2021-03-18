#include "basic_flat_map.h"

#include "details/fixed_vector_bind.h"

namespace Ubpa {
    template<typename Key, typename T, std::size_t N = 16, typename Compare = std::less<Key>>
    using fixed_flat_map = basic_flat_map<details::fixed_vector_bind<N>::template Ttype,
        Key, T, Compare>;
}
