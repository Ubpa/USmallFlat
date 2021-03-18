#pragma once

#include <vector>

namespace Ubpa::details {
    template<template<typename>class TAllocator>
    struct Tvector_bind {
        template<typename T>
        using Ttype = std::vector<T, TAllocator<T>>;
    };
    template<typename Allocator>
    struct vector_bind {
        template<typename T>
        using Ttype = std::vector<T, Allocator>;
    };
}
