#pragma once

#include "../basic_flat_multiset.h"

#include <vector>

namespace Ubpa::pmr {
    template<typename Key, typename Compare = std::less<Key>>
    using flat_multiset = basic_flat_multiset<std::pmr::vector, Key, Compare>;
}
