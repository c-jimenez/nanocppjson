/*
 * Copyright (c) 2025 c-jimenez
 * SPDX-License-Identifier: MIT
 */

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>

#include <nanojsoncpp/nanojsoncpp.h>

using namespace std::string_view_literals;

TEST_SUITE("nanojsoncpp version test suite")
{
    TEST_CASE("Nominal")
    {
        std::string_view version = NANOJSONCPP_MAJOR "." NANOJSONCPP_MINOR "." NANOJSONCPP_FIX;
        CHECK_EQ(version, nanojsoncpp::get_version());
    }
}
