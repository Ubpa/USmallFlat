#include "flat_multiset.h"

#include <vector>

namespace Ubpa {
    template<typename Key, typename Compare = std::less<Key>>
    using pmr_vector_flat_multiset = flat_multiset<Key, std::pmr::vector, Compare>;
}
