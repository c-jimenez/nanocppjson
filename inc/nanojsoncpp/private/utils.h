/*
 * Copyright (c) 2025 c-jimenez
 * SPDX-License-Identifier: MIT
 */

#ifndef NANOJSONCPP_UTILS_H
#define NANOJSONCPP_UTILS_H

#include <cstdint>
#include <string_view>

namespace nanojsoncpp
{

/** @brief Convert an integer value represented as a string to an integer value */
int64_t to_int(const std::string_view& str);

/** @brief Convert a floting point value represented as a string to a floating point value */
double to_float(const std::string_view& str);

} // namespace nanojsoncpp

#endif // NANOJSONCPP_UTILS_H
