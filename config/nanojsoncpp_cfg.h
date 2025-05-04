/*
 * Copyright (c) 2025 c-jimenez
 * SPDX-License-Identifier: MIT
 */

#ifndef NANOJSONCPP_CFG_H
#define NANOJSONCPP_CFG_H

/** @brief Use C++17 std::from_char() and std::to_char() for string to/from numeric types conversion. 
 *         These are the most efficient standard functions. 
 *         Disable only if your compiler does not support them or if the induced memory footprint is too big.
 */
#define NANOJSONCPP_USE_CPP17_FROM_CHARS_TO_CHARS

/** @brief Data type to store the size of json value as one of the following types :
 *         uint16_t => max json string size = 65535 bytes
 *         uint32_t => max json string size ~4GBytes
 */
#define NANOJSONCPP_SIZE_T uint16_t

/** @brief Data type to store the smallest/biggest integer represented by a json value as one of the following types :
 *         int32_t => [-2147483648;2147483647]
 *         int64_t => [-9223372036854775808;9223372036854775807]
 */
#define NANOJSONCPP_INT_T int64_t

/** @brief Data type to store the smallest/biggest floating point represented by a json value as one of the following types :
 *         float => [-3.40282e+38;3.40282e+38]
 *         double => [-1.79769e+308;1.79769e+308]
 */
#define NANOJSONCPP_FLOAT_T double

#endif // NANOJSONCPP_CFG_H
