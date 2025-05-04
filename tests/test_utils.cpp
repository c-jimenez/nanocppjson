/*
 * Copyright (c) 2025 c-jimenez
 * SPDX-License-Identifier: MIT
 */

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>

#include <nanojsoncpp/private/utils.h>

#include <limits>

using namespace std::string_view_literals;

TEST_SUITE("nanojsoncpp utilities test suite")
{
    TEST_CASE("to_int()")
    {
        // Invalid values
        CHECK_EQ(nanojsoncpp::to_int(""), 0);
        CHECK_EQ(nanojsoncpp::to_int("-"), 0);
        CHECK_EQ(nanojsoncpp::to_int("+123"), 0);
#ifdef NANOJSONCPP_USE_CPP17_FROM_CHARS_TO_CHARS
        CHECK_EQ(nanojsoncpp::to_int("12x3"), 12);
#else
        CHECK_EQ(nanojsoncpp::to_int("12x3"), 0);
#endif

        // Valid values
        CHECK_EQ(nanojsoncpp::to_int("0"), 0);
        CHECK_EQ(nanojsoncpp::to_int("-0"), 0);
        CHECK_EQ(nanojsoncpp::to_int("1234567890"), 1234567890);
        CHECK_EQ(nanojsoncpp::to_int("-1234567890"), -1234567890);
        CHECK_EQ(nanojsoncpp::to_int(std::to_string(std::numeric_limits<nanojsoncpp::json_int_t>::max())),
                 std::numeric_limits<nanojsoncpp::json_int_t>::max());
        CHECK_EQ(nanojsoncpp::to_int(std::to_string(std::numeric_limits<nanojsoncpp::json_int_t>::min())),
                 std::numeric_limits<nanojsoncpp::json_int_t>::min());
    }

    TEST_CASE("to_float()")
    {
        // Invalid values
        CHECK_EQ(nanojsoncpp::to_float(""), 0.);
        CHECK_EQ(nanojsoncpp::to_float("-"), 0.);
        CHECK_EQ(nanojsoncpp::to_float("."), 0.);

        // Valid values
        CHECK_EQ(nanojsoncpp::to_float("0"), 0.);
        CHECK_EQ(nanojsoncpp::to_float("-0"), 0.);
        CHECK_EQ(nanojsoncpp::to_float("123."), 123.);
        CHECK_EQ(nanojsoncpp::to_float("-123."), -123.);
        CHECK_EQ(nanojsoncpp::to_float("1234567890.54321"), static_cast<nanojsoncpp::json_float_t>(1234567890.54321));
        CHECK_EQ(nanojsoncpp::to_float("-1234567890.54321"), static_cast<nanojsoncpp::json_float_t>(-1234567890.54321));
    }

    TEST_CASE("to_string(json_int_t)")
    {
        size_t s = 0u;

        // Buffer large enough
        char tmp1[30];
        s = nanojsoncpp::to_string(static_cast<nanojsoncpp::json_int_t>(0), tmp1);
        CHECK_EQ(std::string_view(tmp1, s), "0"sv);

        char                    tmp2[19u];
        nanojsoncpp::json_int_t max = std::numeric_limits<nanojsoncpp::json_int_t>::max();
        s                           = nanojsoncpp::to_string(max, tmp2);
        CHECK_EQ(std::string_view(tmp2, s), std::to_string(max));

        char                    tmp3[20u];
        nanojsoncpp::json_int_t min = std::numeric_limits<nanojsoncpp::json_int_t>::min() + 1;
        s                           = nanojsoncpp::to_string(min, tmp3);
        CHECK_EQ(std::string_view(tmp3, s), std::to_string(min));

        // Buffer too small
        s = nanojsoncpp::to_string(static_cast<nanojsoncpp::json_int_t>(0), {});
        CHECK_EQ(s, 0);

        s = nanojsoncpp::to_string(static_cast<nanojsoncpp::json_int_t>(-1), {});
        CHECK_EQ(s, 0);

        char tmp4[9u];
        s = nanojsoncpp::to_string(max, tmp4);
        CHECK_EQ(s, 0);

        char tmp5[9u];
        s = nanojsoncpp::to_string(min, tmp5);
        CHECK_EQ(s, 0);
    }
}
