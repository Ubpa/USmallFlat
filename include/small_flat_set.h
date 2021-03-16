#include "fixed_vector.h"
#include "flat_set.h"

namespace Ubpa {
    template<typename Key, std::size_t N, typename Compare = std::less<Key>>
    using fixed_flat_set = flat_set<fixed_vector<Key, N>, Compare>;
}
#include "small_vector.h"
#include "flat_set.h"

namespace Ubpa {
    template<typename Key, std::size_t N = 16, typename Compare = std::less<Key>>
    using small_flat_set = flat_set<small_vector<Key, N>, Compare>;
}
