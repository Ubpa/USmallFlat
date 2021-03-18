#pragma once

#include "../basic_flat_set.h"

#include <vector>

namespace Ubpa::pmr {
    template<typename Key, typename Compare = std::less<Key>>
    using flat_set = basic_flat_set<std::pmr::vector, Key, Compare>;
}
