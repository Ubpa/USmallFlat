#include "flat_multimap.h"

#include <vector>

namespace Ubpa {
    template<typename Key, typename Compare = std::less<Key>>
    using pmr_vector_flat_multimap = flat_multimap<Key, std::pmr::vector, Compare>;
}
