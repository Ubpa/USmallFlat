#pragma once

#include "../basic_flat_multimap.hpp"

#include <vector>

namespace Ubpa::pmr {
    template<typename Key, typename T, typename Compare = std::less<Key>>
    using flat_multimap = basic_flat_multimap<std::pmr::vector, Key, T, Compare>;
}
