/*
 * Copyright (c) 2025 c-jimenez
 * SPDX-License-Identifier: MIT
 */

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>

#include <nanojsoncpp/private/utils.h>

#include <limits>

#include <vector>

TEST_SUITE("nanojsoncpp utilities test suite")
{
    TEST_CASE("to_int()")
    {
        // Invalid values
        CHECK_EQ(nanojsoncpp::to_int(""), 0);
        CHECK_EQ(nanojsoncpp::to_int("-"), 0);
        CHECK_EQ(nanojsoncpp::to_int("+123"), 0);
        CHECK_EQ(nanojsoncpp::to_int("12x3"), 0);

        // Valid values
        CHECK_EQ(nanojsoncpp::to_int("0"), 0);
        CHECK_EQ(nanojsoncpp::to_int("-0"), 0);
        CHECK_EQ(nanojsoncpp::to_int("1234567890"), 1234567890);
        CHECK_EQ(nanojsoncpp::to_int("-1234567890"), -1234567890);
        CHECK_EQ(nanojsoncpp::to_int("9223372036854775807"), std::numeric_limits<int64_t>::max());
        CHECK_EQ(nanojsoncpp::to_int("-9223372036854775808"), std::numeric_limits<int64_t>::min());
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
        CHECK_EQ(nanojsoncpp::to_float("1234567890.54321"), 1234567890.54321);
        CHECK_EQ(nanojsoncpp::to_float("-1234567890.54321"), -1234567890.54321);
    }
}
