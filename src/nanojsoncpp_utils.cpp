/*
 * Copyright (c) 2025 c-jimenez
 * SPDX-License-Identifier: MIT
 */

#include <nanojsoncpp/nanojsoncpp.h>

#ifdef NANOJSONCPP_USE_CPP17_FROM_CHARS_TO_CHARS
#include <charconv>
#endif

namespace nanojsoncpp
{

/** @brief Convert an integer value represented as a string to an integer value */
json_int_t to_int(const std::string_view& str)
{
    json_int_t val = 0;

#ifdef NANOJSONCPP_USE_CPP17_FROM_CHARS_TO_CHARS
    // Implementation using std::from_chars

    std::from_chars(str.data(), str.data() + str.size(), val, 10);

#else
    // Custom implementation

    // Check empty value
    if (!str.empty())
    {
        // Check sign
        json_int_t       sign = 1;
        std::string_view sv   = str;
        if (sv[0] == '-')
        {
            sign = -1;
            sv   = sv.substr(1u);
        }

        // Convert value
        for (char c : sv)
        {
            if (c >= '0' && c <= '9')
            {
                // Increment value
                val = val * 10u + (c - '0');
            }
            else
            {
                // Invalid value
                val = 0u;
                break;
            }
        }

        // Add sign
        val *= sign;
    }

#endif // NANOJSONCPP_USE_CPP17_FROM_CHARS_TO_CHARS

    return val;
}

/** @brief Convert a floting point value represented as a string to a floating point value */
json_float_t to_float(const std::string_view& str)
{
    json_float_t val = 0.;

#ifdef NANOJSONCPP_USE_CPP17_FROM_CHARS_TO_CHARS
    // Implementation using std::from_chars

    std::from_chars(str.data(), str.data() + str.size(), val);

#else
    // Custom implementation

    // Check empty value
    if (!str.empty())
    {
        // Check sign
        json_float_t     sign = 1.;
        std::string_view sv   = str;
        if (sv[0] == '-')
        {
            sign = -1.;
            sv   = sv.substr(1u);
        }

        // Look for decimal point
        size_t index_point = 0u;
        for (char c : sv)
        {
            if (c == '.')
            {
                break;
            }
            else
            {
                index_point++;
            }
        }

        // Get integer part
        json_float_t int_part = static_cast<json_float_t>(to_int(sv.substr(0u, index_point)));

        // Get the fractionnal part
        json_float_t frac_part = 0.;
        if ((index_point + 1u) < sv.size())
        {
            std::string_view sv_frac = sv.substr(index_point + 1u);
            frac_part                = static_cast<json_float_t>(to_int(sv_frac));

            json_float_t div = 1.;
            for ([[maybe_unused]] char c : sv_frac)
            {
                div *= static_cast<json_float_t>(10.);
            }
            frac_part = frac_part / div;
        }

        // Compute value
        val = (int_part + frac_part) * sign;
    }

#endif // NANOJSONCPP_USE_CPP17_FROM_CHARS_TO_CHARS

    return val;
}

/** @brief Convert an integer value to a string */
size_t to_string(json_int_t val, const std::span<char>& buff)
{
    size_t ret = 0u;

#ifdef NANOJSONCPP_USE_CPP17_FROM_CHARS_TO_CHARS
    // Implementation using std::to_chars

    auto res = std::to_chars(buff.data(), buff.data() + buff.size(), val, 10);
    if (res.ec != std::errc::value_too_large)
    {
        ret = static_cast<size_t>(res.ptr - buff.data());
    }

#else
    // Custom implementation

    // Base 10 digits in ASCII chars
    static const char ascii_base[] = "0123456789";

    // Check sign
    char*  buffer = buff.data();
    size_t count  = 0u;
    if ((val < 0) && !buff.empty())
    {
        *buffer = '-';
        buffer++;
        count = 1u;
        val   = std::abs(val);
    }

    // Convert value
    json_int_t left       = val;
    char*      inv_buffer = buffer;
    size_t     max        = buff.size();
    while ((count != max) && (left >= 10))
    {
        json_int_t q = left / 10;
        json_int_t r = left - q * 10;
        *inv_buffer  = ascii_base[r];
        inv_buffer++;
        count++;
        left = q;
    }
    if ((count != max) && (left < 10))
    {
        // Last digit
        *inv_buffer = ascii_base[left];
        count++;

        // Invert buffer contents
        while (inv_buffer > buffer)
        {
            char tmp      = (*buffer);
            (*buffer)     = (*inv_buffer);
            (*inv_buffer) = tmp;
            inv_buffer--;
            buffer++;
        }

        ret = count;
    }

#endif // NANOJSONCPP_USE_CPP17_FROM_CHARS_TO_CHARS

    return ret;
}

/** @brief Convert a floating point value to a string */
size_t to_string(json_float_t val, const std::span<char>& buff)
{
    size_t ret = 0u;

#ifdef NANOJSONCPP_USE_CPP17_FROM_CHARS_TO_CHARS
    // Implementation using std::to_chars

    auto res = std::to_chars(buff.data(), buff.data() + buff.size(), val);
    if (res.ec != std::errc::value_too_large)
    {
        ret = static_cast<size_t>(res.ptr - buff.data());
    }

#else
    // Implementation using snprintf

    size_t res = static_cast<size_t>(snprintf(buff.data(), buff.size(), "%.6f", val));
    if (res <= buff.size())
    {
        ret = res;
    }

#endif // NANOJSONCPP_USE_CPP17_FROM_CHARS_TO_CHARS

    return ret;
}

} // namespace nanojsoncpp
