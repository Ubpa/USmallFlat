#include "flat_map.h"

#include <vector>

namespace Ubpa {
    template<typename Key, typename Compare = std::less<Key>>
    using pmr_vector_flat_map = flat_map<Key, std::pmr::vector, Compare>;
}
