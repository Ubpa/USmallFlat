#include "flat_multiset.h"

#include "fixed_vector.h"

namespace Ubpa::details {
    template<std::size_t N>
    struct fixed_flat_multiset_helper {
        template<typename T>
        using Ttype = fixed_vector<T, N>;
    };
}

namespace Ubpa {
    template<typename Key, std::size_t N = 16, typename Compare = std::less<Key>>
    using fixed_flat_multiset = flat_multiset<Key,
        details::fixed_flat_multiset_helper<N>::template Ttype,
        Compare>;
}
