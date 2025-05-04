/*
 * Copyright (c) 2025 c-jimenez
 * SPDX-License-Identifier: MIT
 */

#include <nanojsoncpp/private/utils.h>

namespace nanojsoncpp
{

/** @brief Convert an integer value represented as a string to an integer value */
int64_t to_int(const std::string_view& str)
{
    int64_t val = 0u;

    // Check empty value
    if (!str.empty())
    {
        // Check sign
        int64_t          sign = 1;
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

    return val;
}

/** @brief Convert a floting point value represented as a string to a floating point value */
double to_float(const std::string_view& str)
{
    double val = 0u;

    // Check empty value
    if (!str.empty())
    {
        // Check sign
        double           sign = 1.;
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
        double int_part = static_cast<double>(to_int(sv.substr(0u, index_point)));

        // Get the fractionnal part
        double frac_part = 0.;
        if ((index_point + 1u) < sv.size())
        {
            std::string_view sv_frac = sv.substr(index_point + 1u);
            frac_part                = static_cast<double>(to_int(sv_frac));

            double div = 1.;
            for ([[maybe_unused]] char c : sv_frac)
            {
                div *= 10.;
            }
            frac_part = frac_part / div;
        }

        // Compute value
        val = (int_part + frac_part) * sign;
    }

    return val;
}

} // namespace nanojsoncpp
