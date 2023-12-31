#pragma once

#include <type_traits>
#include <concepts>

template <typename T>
concept is_container_type = requires(T x) { x[0]; x.begin(); x.end(); };