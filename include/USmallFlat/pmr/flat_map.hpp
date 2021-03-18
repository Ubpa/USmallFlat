#pragma once

#include "../basic_flat_map.hpp"

#include <vector>

namespace Ubpa::pmr {
    template<typename Key, typename T, typename Compare = std::less<Key>, typename Allocator = std::allocator<std::pair<Key, T>>>
    using flat_map = basic_flat_map<std::pmr::vector, Key, T, Compare>;
}
