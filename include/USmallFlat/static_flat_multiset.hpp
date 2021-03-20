#include "basic_flat_multiset.hpp"

#include "details/static_vector_bind.hpp"

namespace Ubpa {
    template<typename Key, std::size_t N = 16, typename Compare = std::less<Key>>
    using static_flat_multiset = basic_flat_multiset<details::static_vector_bind<N>::template Ttype,
        Key, Compare>;
}
