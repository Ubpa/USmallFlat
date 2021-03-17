#include "flat_set.h"

#include "fixed_vector.h"

namespace Ubpa::details {
    template<std::size_t N>
    struct fixed_flat_set_helper {
        template<typename T>
        using Ttype = fixed_vector<T, N>;
    };
}

namespace Ubpa {
    template<typename Key, std::size_t N = 16, typename Compare = std::less<Key>>
    using fixed_flat_set = flat_set<Key,
        details::fixed_flat_set_helper<N>::template Ttype,
        Compare>;
}
