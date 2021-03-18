#include "basic_flat_set.h"

#include "details/fixed_vector_bind.h"

namespace Ubpa {
    template<typename Key, std::size_t N = 16, typename Compare = std::less<Key>>
    using fixed_flat_set = basic_flat_set<details::fixed_vector_bind<N>::template Ttype,
        Key, Compare>;
}
