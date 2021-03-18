#pragma once

#include "../basic_small_vector.hpp"

namespace Ubpa::details {
    template<template<typename>class Vector, std::size_t N>
    struct small_vector_bind {
        template<typename T>
        using Ttype = basic_small_vector<Vector, T, N>;
    };
}
