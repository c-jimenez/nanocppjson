/*
 * Copyright (c) 2025 c-jimenez
 * SPDX-License-Identifier: MIT
 */

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>

#include <nanojsoncpp/private/string.h>

using namespace std::string_view_literals;

TEST_SUITE("nanojsoncpp::string test suite")
{
    TEST_CASE("Allocator")
    {
        SUBCASE("Nominal")
        {
            nanojsoncpp::static_allocator<100u> allocator;

            CHECK_EQ(allocator.available(), 100u);
            CHECK_EQ(allocator.size(), 100u);

            double* data_d = allocator.alloc<double>(10u);
            REQUIRE(data_d);
            CHECK_EQ(allocator.available(), 100u - (10u * sizeof(double)));
            CHECK_EQ(allocator.size(), 100u);

            int* data_i = allocator.alloc<int>(0u);
            REQUIRE(data_i);
            CHECK_EQ(allocator.available(), 100u - (10u * sizeof(double)));
            CHECK_EQ(allocator.size(), 100u);

            uint8_t* data_u8 = allocator.alloc<uint8_t>(21u);
            CHECK_FALSE(data_u8);
            CHECK_EQ(allocator.available(), 100u - (10u * sizeof(double)));
            CHECK_EQ(allocator.size(), 100u);

            data_u8 = allocator.alloc<uint8_t>(20u);
            REQUIRE(data_u8);
            CHECK_EQ(allocator.available(), 0u);
            CHECK_EQ(allocator.size(), 100u);

            float* data_f = allocator.alloc<float>(1u);
            CHECK_FALSE(data_f);
        }

        SUBCASE("0-sized allocator")
        {
            nanojsoncpp::static_allocator<0u> allocator;

            CHECK_EQ(allocator.available(), 0u);
            CHECK_EQ(allocator.size(), 0u);

            int* data_i = allocator.alloc<int>(0u);
            CHECK_FALSE(data_i);
        }
    }

    TEST_CASE("String")
    {
        SUBCASE("Dynamic allocation")
        {
            nanojsoncpp::static_allocator<11u> allocator;

            nanojsoncpp::string str1;
            CHECK_EQ(str1.get(), ""sv);
            CHECK(str1.set("Yip!"sv, allocator));
            CHECK_EQ(str1, "Yip!"sv);

            size_t available = allocator.available();
            CHECK_FALSE(str1.set("Yop!", allocator));
            CHECK_EQ(str1, "Yip!"sv);
            CHECK_EQ(available, allocator.available());

            CHECK_FALSE(str1.set("Static string"sv));
            CHECK_EQ(str1, "Yip!"sv);
            CHECK_EQ(available, allocator.available());

            nanojsoncpp::string str2;
            CHECK(str2.set("Yop!"sv, allocator));
            CHECK_EQ(str2, "Yop!"sv);

            CHECK_EQ(allocator.available(), 3u);

            nanojsoncpp::string str3;
            CHECK_FALSE(str3.set("Arg!"sv, allocator));
            CHECK(str3.set(";-)"sv, allocator));
            CHECK_EQ(str3, ";-)"sv);

            CHECK_EQ(allocator.available(), 0u);
        }

        SUBCASE("Dynamic allocation - empty string")
        {
            nanojsoncpp::static_allocator<1u> allocator;

            nanojsoncpp::string str1;
            CHECK_EQ(str1.get(), ""sv);
            CHECK(str1.set(""sv, allocator));
            CHECK_EQ(str1, ""sv);

            CHECK_EQ(allocator.available(), 1u);
        }

        SUBCASE("Static allocation")
        {
            nanojsoncpp::string str1;
            CHECK_EQ(str1.get(), ""sv);

            CHECK(str1.set("My string!"sv));
            CHECK_EQ(str1, "My string!"sv);

            CHECK_FALSE(str1.set("Another string!"sv));
            CHECK_EQ(str1, "My string!"sv);

            nanojsoncpp::static_allocator<10u> allocator;
            CHECK_FALSE(str1.set("Another string!"sv, allocator));
            CHECK_EQ(str1, "My string!"sv);

            nanojsoncpp::string str2;
            CHECK_EQ(str2.get(), ""sv);

            CHECK(str2.set(""sv));
            CHECK_EQ(str2, ""sv);

            CHECK(str2.set("Another string!"sv));
            CHECK_EQ(str2, "Another string!"sv);
        }
    }
}
