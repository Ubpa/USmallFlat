#include "flat_set.h"

#include <vector>

namespace Ubpa {
    template<typename Key, typename Compare = std::less<Key>, typename Allocator = std::allocator<Key>>
    using vector_flat_set = flat_set<std::vector<Key, Allocator>, Compare>;
}
