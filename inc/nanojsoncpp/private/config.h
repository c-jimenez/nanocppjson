/*
 * Copyright (c) 2025 c-jimenez
 * SPDX-License-Identifier: MIT
 */

#ifndef NANOJSONCPP_CONFIG_H
#define NANOJSONCPP_CONFIG_H

// Standard headers
#include <cstdint>
#include <limits>
#include <type_traits>

// Generated user config file
#include <nanojsoncpp_cfg.h>

namespace nanojsoncpp
{

/** @brief Data type to store the size of json value as one of the following types :
 *         uint16_t => max json string size = 65535 bytes
 *         uint32_t => max json string size ~4GBytes
 */
using json_size_t = NANOJSONCPP_SIZE_T;

/** @brief Data type to store the smallest/biggest integer represented by a json value as one of the following types :
 *         int32_t => [-2147483648;2147483647]
 *         int64_t => [-9223372036854775808;9223372036854775807]
 */
using json_int_t = NANOJSONCPP_INT_T;

/** @brief Data type to store the smallest/biggest floating point represented by a json value as one of the following types :
 *         float => [-3.40282e+38;3.40282e+38]
 *         double => [-1.79769e+308;1.79769e+308]
 */
using json_float_t = NANOJSONCPP_FLOAT_T;

// Check configuration
static_assert(std::is_integral_v<json_size_t> && std::is_unsigned_v<json_size_t> && (sizeof(json_size_t) >= sizeof(uint16_t)));
static_assert(std::is_integral_v<json_int_t> && std::is_signed_v<json_int_t> && (sizeof(json_int_t) >= sizeof(int32_t)));
static_assert(std::is_floating_point_v<json_float_t> && (sizeof(json_float_t) >= sizeof(float)));

} // namespace nanojsoncpp

#endif // NANOJSONCPP_CONFIG_H
