#pragma once

template <typename KEY_T, typename DATA_T>
struct AVLTreeTraits {
    using Comparator = int(*)(void*, const KEY_T&, const KEY_T&);

    using DataProcessor = bool(*)(void*, const KEY_T&, const DATA_T&);
};
