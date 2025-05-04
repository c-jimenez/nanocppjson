/*
 * Copyright (c) 2025 c-jimenez
 * SPDX-License-Identifier: MIT
 */

#ifndef NANOJSONCPP_UTILS_H
#define NANOJSONCPP_UTILS_H

// Standard headers
#include <cstdint>
#include <string_view>

// C++20 headers
#if __cplusplus > 201703L
#include <span>
#else
#include <nanojsoncpp/private/span.h>
#endif // __cplusplus > 201703L

// Internal headers
#include <nanojsoncpp/private/config.h>

namespace nanojsoncpp
{

/** @brief Convert an integer value represented as a string to an integer value */
json_int_t to_int(const std::string_view& str);

/** @brief Convert a floting point value represented as a string to a floating point value */
json_float_t to_float(const std::string_view& str);

/** @brief Convert an integer value to a string */
size_t to_string(json_int_t val, const std::span<char>& buff);

/** @brief Convert a floating point value to a string */
size_t to_string(json_float_t val, const std::span<char>& buff);

} // namespace nanojsoncpp

#endif // NANOJSONCPP_UTILS_H
