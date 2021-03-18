#include "flat_map.h"

#include "fixed_vector.h"

namespace Ubpa::details {
    template<std::size_t N>
    struct fixed_flat_map_helper {
        template<typename T>
        using Ttype = fixed_vector<T, N>;
    };
}

namespace Ubpa {
    template<typename Key, typename T, std::size_t N = 16, typename Compare = std::less<Key>>
    using fixed_flat_map = flat_map<Key, T,
        details::fixed_flat_map_helper<N>::template Ttype,
        Compare>;
}
