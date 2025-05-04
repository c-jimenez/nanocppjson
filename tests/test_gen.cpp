/*
 * Copyright (c) 2025 c-jimenez
 * SPDX-License-Identifier: MIT
 */

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>

#include <nanojsoncpp/nanojsoncpp.h>

using namespace std::string_view_literals;

TEST_SUITE("json generation test suite")
{
    TEST_CASE("Null value")
    {
        SUBCASE("Member value")
        {
            char tmp1[20];
            auto left = nanojsoncpp::generate_value("null_value"sv, tmp1);
            CHECK_FALSE(left.empty());
            CHECK_EQ(R"("null_value":null,)"sv, std::string_view(tmp1, sizeof(tmp1) - left.size()));

            char tmp2[3];
            auto left2 = nanojsoncpp::generate_value("null_value"sv, tmp2);
            CHECK(left2.empty());
        }

        SUBCASE("Array value")
        {
            char tmp1[10];
            auto left = nanojsoncpp::generate_value(""sv, tmp1);
            CHECK_FALSE(left.empty());
            CHECK_EQ("null,"sv, std::string_view(tmp1, sizeof(tmp1) - left.size()));

            char tmp2[3];
            auto left2 = nanojsoncpp::generate_value(""sv, tmp2);
            CHECK(left2.empty());
        }
    }

    TEST_CASE("Boolean value")
    {
        SUBCASE("Member value")
        {
            char tmp1[20];
            auto left = nanojsoncpp::generate_value("bool_value"sv, true, tmp1);
            CHECK_FALSE(left.empty());
            CHECK_EQ(R"("bool_value":true,)"sv, std::string_view(tmp1, sizeof(tmp1) - left.size()));

            char tmp2[20];
            auto left2 = nanojsoncpp::generate_value("bool_value"sv, false, tmp2);
            CHECK_FALSE(left2.empty());
            CHECK_EQ(R"("bool_value":false,)"sv, std::string_view(tmp2, sizeof(tmp2) - left2.size()));

            char tmp3[15];
            auto left3 = nanojsoncpp::generate_value("bool_value"sv, false, tmp3);
            CHECK(left3.empty());
        }

        SUBCASE("Array value")
        {
            char tmp1[20];
            auto left = nanojsoncpp::generate_value(""sv, true, tmp1);
            CHECK_FALSE(left.empty());
            CHECK_EQ("true,"sv, std::string_view(tmp1, sizeof(tmp1) - left.size()));

            char tmp2[20];
            auto left2 = nanojsoncpp::generate_value(""sv, false, tmp2);
            CHECK_FALSE(left2.empty());
            CHECK_EQ("false,"sv, std::string_view(tmp2, sizeof(tmp2) - left2.size()));

            char tmp3[3];
            auto left3 = nanojsoncpp::generate_value(""sv, false, tmp3);
            CHECK(left3.empty());
        }
    }

    TEST_CASE("String value")
    {
        SUBCASE("Member value")
        {
            char tmp1[30];
            auto left = nanojsoncpp::generate_value("string_value"sv, "My string!"sv, tmp1);
            CHECK_FALSE(left.empty());
            CHECK_EQ(R"("string_value":"My string!",)"sv, std::string_view(tmp1, sizeof(tmp1) - left.size()));

            char tmp2[30];
            auto left2 = nanojsoncpp::generate_value("string_value"sv, ""sv, tmp2);
            CHECK_FALSE(left2.empty());
            CHECK_EQ(R"("string_value":"",)"sv, std::string_view(tmp2, sizeof(tmp2) - left2.size()));

            char tmp3[15];
            auto left3 = nanojsoncpp::generate_value("string_value"sv, "My string!"sv, tmp3);
            CHECK(left3.empty());

            char tmp4[51];
            auto left4 = nanojsoncpp::generate_value("string_value"sv, "\"My\t\fescaped\b\nstring\\!/\r"sv, tmp4);
            CHECK_FALSE(left4.empty());
            CHECK_EQ(R"("string_value":"\"My\t\fescaped\b\nstring\\!\/\r",)"sv, std::string_view(tmp4, sizeof(tmp4) - left4.size()));

            char tmp5[48];
            auto left5 = nanojsoncpp::generate_value("string_value"sv, "\"My\t\fescaped\b\nstring\\!/\r"sv, tmp5);
            CHECK(left5.empty());
        }

        SUBCASE("Array value")
        {
            char tmp1[20];
            auto left = nanojsoncpp::generate_value(""sv, "My string!"sv, tmp1);
            CHECK_FALSE(left.empty());
            CHECK_EQ(R"("My string!",)"sv, std::string_view(tmp1, sizeof(tmp1) - left.size()));

            char tmp2[20];
            auto left2 = nanojsoncpp::generate_value(""sv, ""sv, tmp2);
            CHECK_FALSE(left2.empty());
            CHECK_EQ(R"("",)"sv, std::string_view(tmp2, sizeof(tmp2) - left2.size()));

            char tmp3[3];
            auto left3 = nanojsoncpp::generate_value(""sv, "My string!"sv, tmp3);
            CHECK(left3.empty());
        }
    }

    TEST_CASE("Integer value")
    {
        SUBCASE("Member value")
        {
            char tmp1[30];
            auto left = nanojsoncpp::generate_value("int_value"sv, static_cast<nanojsoncpp::json_int_t>(987654321), tmp1);
            CHECK_FALSE(left.empty());
            CHECK_EQ(R"("int_value":987654321,)"sv, std::string_view(tmp1, sizeof(tmp1) - left.size()));

            char tmp2[30];
            auto left2 = nanojsoncpp::generate_value("int_value"sv, static_cast<nanojsoncpp::json_int_t>(-987654321), tmp2);
            CHECK_FALSE(left2.empty());
            CHECK_EQ(R"("int_value":-987654321,)"sv, std::string_view(tmp2, sizeof(tmp2) - left2.size()));

            char tmp3[15];
            auto left3 = nanojsoncpp::generate_value("int_value"sv, static_cast<nanojsoncpp::json_int_t>(987654321), tmp3);
            CHECK(left3.empty());
        }

        SUBCASE("Array value")
        {
            char tmp1[20];
            auto left = nanojsoncpp::generate_value(""sv, static_cast<nanojsoncpp::json_int_t>(987654321), tmp1);
            CHECK_FALSE(left.empty());
            CHECK_EQ("987654321,"sv, std::string_view(tmp1, sizeof(tmp1) - left.size()));

            char tmp2[20];
            auto left2 = nanojsoncpp::generate_value(""sv, static_cast<nanojsoncpp::json_int_t>(-987654321), tmp2);
            CHECK_FALSE(left2.empty());
            CHECK_EQ("-987654321,"sv, std::string_view(tmp2, sizeof(tmp2) - left2.size()));

            char tmp3[3];
            auto left3 = nanojsoncpp::generate_value(""sv, static_cast<nanojsoncpp::json_int_t>(987654321), tmp3);
            CHECK(left3.empty());
        }
    }

    TEST_CASE("Floating point value")
    {
        SUBCASE("Member value")
        {
            char tmp1[30];
            auto left = nanojsoncpp::generate_value("float_value"sv, static_cast<nanojsoncpp::json_float_t>(3.14), tmp1);
            CHECK_FALSE(left.empty());
#ifdef NANOJSONCPP_USE_CPP17_FROM_CHARS_TO_CHARS
            CHECK_EQ(R"("float_value":3.14,)"sv, std::string_view(tmp1, sizeof(tmp1) - left.size()));
#else
            CHECK_EQ(R"("float_value":3.140000,)"sv, std::string_view(tmp1, sizeof(tmp1) - left.size()));
#endif

            char tmp2[30];
            auto left2 = nanojsoncpp::generate_value("float_value"sv, static_cast<nanojsoncpp::json_float_t>(-3.14), tmp2);
            CHECK_FALSE(left2.empty());
#ifdef NANOJSONCPP_USE_CPP17_FROM_CHARS_TO_CHARS
            CHECK_EQ(R"("float_value":-3.14,)"sv, std::string_view(tmp2, sizeof(tmp2) - left2.size()));
#else
            CHECK_EQ(R"("float_value":-3.140000,)"sv, std::string_view(tmp2, sizeof(tmp2) - left2.size()));
#endif

            char tmp3[15];
            auto left3 = nanojsoncpp::generate_value("float_value"sv, static_cast<nanojsoncpp::json_float_t>(3.14), tmp3);
            CHECK(left3.empty());
        }

        SUBCASE("Array value")
        {
            char tmp1[20];
            auto left = nanojsoncpp::generate_value(""sv, static_cast<nanojsoncpp::json_float_t>(3.14), tmp1);
            CHECK_FALSE(left.empty());
#ifdef NANOJSONCPP_USE_CPP17_FROM_CHARS_TO_CHARS
            CHECK_EQ("3.14,"sv, std::string_view(tmp1, sizeof(tmp1) - left.size()));
#else
            CHECK_EQ("3.140000,"sv, std::string_view(tmp1, sizeof(tmp1) - left.size()));
#endif

            char tmp2[20];
            auto left2 = nanojsoncpp::generate_value(""sv, static_cast<nanojsoncpp::json_float_t>(-3.14), tmp2);
            CHECK_FALSE(left2.empty());
#ifdef NANOJSONCPP_USE_CPP17_FROM_CHARS_TO_CHARS
            CHECK_EQ("-3.14,"sv, std::string_view(tmp2, sizeof(tmp2) - left2.size()));
#else
            CHECK_EQ("-3.140000,"sv, std::string_view(tmp2, sizeof(tmp2) - left2.size()));
#endif

            char tmp3[3];
            auto left3 = nanojsoncpp::generate_value(""sv, static_cast<nanojsoncpp::json_float_t>(3.14), tmp3);
            CHECK(left3.empty());
        }
    }

    TEST_CASE("Array value")
    {
        char tmp1[30];
        auto left = nanojsoncpp::generate_array_begin("array_value"sv, tmp1);
        CHECK_FALSE(left.empty());
        CHECK_EQ(R"("array_value":[)"sv, std::string_view(tmp1, sizeof(tmp1) - left.size()));

        char tmp2[30];
        auto left2 = nanojsoncpp::generate_array_end(tmp2);
        CHECK_FALSE(left2.empty());
        CHECK_EQ("],"sv, std::string_view(tmp2, sizeof(tmp2) - left2.size()));

        char tmp3[15];
        auto left3 = nanojsoncpp::generate_array_begin(""sv, tmp3);
        CHECK_FALSE(left3.empty());
        CHECK_EQ("["sv, std::string_view(tmp3, sizeof(tmp3) - left3.size()));

        std::array<char, 0u> tmp4;
        auto                 left4 = nanojsoncpp::generate_array_begin("array_value"sv, tmp4);
        CHECK(left4.empty());

        std::array<char, 0u> tmp5;
        auto                 left5 = nanojsoncpp::generate_array_begin(""sv, tmp5);
        CHECK(left5.empty());

        std::array<char, 0u> tmp6;
        auto                 left6 = nanojsoncpp::generate_array_end(tmp6);
        CHECK(left6.empty());

        char tmp7[50u];
        auto left7 = nanojsoncpp::generate_array_begin(""sv, tmp7);
        left7      = nanojsoncpp::generate_value("", 9876543210, left7);
        left7      = nanojsoncpp::generate_array_end(left7);
        CHECK_EQ(R"([9876543210],)"sv, std::string_view(tmp7, sizeof(tmp7) - left7.size()));
    }

    TEST_CASE("Object value")
    {
        char tmp1[30];
        auto left = nanojsoncpp::generate_object_begin("object_value"sv, tmp1);
        CHECK_FALSE(left.empty());
        CHECK_EQ(R"("object_value":{)"sv, std::string_view(tmp1, sizeof(tmp1) - left.size()));

        char tmp2[30];
        auto left2 = nanojsoncpp::generate_object_end(tmp2);
        CHECK_FALSE(left2.empty());
        CHECK_EQ("},"sv, std::string_view(tmp2, sizeof(tmp2) - left2.size()));

        char tmp3[15];
        auto left3 = nanojsoncpp::generate_object_begin(""sv, tmp3);
        CHECK_FALSE(left3.empty());
        CHECK_EQ("{"sv, std::string_view(tmp3, sizeof(tmp3) - left3.size()));

        std::array<char, 0u> tmp4;
        auto                 left4 = nanojsoncpp::generate_object_begin("object_value"sv, tmp4);
        CHECK(left4.empty());

        std::array<char, 0u> tmp5;
        auto                 left5 = nanojsoncpp::generate_object_begin(""sv, tmp5);
        CHECK(left5.empty());

        std::array<char, 0u> tmp6;
        auto                 left6 = nanojsoncpp::generate_object_end(tmp6);
        CHECK(left6.empty());

        char tmp7[50u];
        auto left7 = nanojsoncpp::generate_object_begin(""sv, tmp7);
        left7      = nanojsoncpp::generate_value("my_value", 9876543210, left7);
        left7      = nanojsoncpp::generate_object_end(left7);
        CHECK_EQ(R"({"my_value":9876543210},)"sv, std::string_view(tmp7, sizeof(tmp7) - left7.size()));
    }

    TEST_CASE("JSON string")
    {
        char tmp[100];

        auto left = nanojsoncpp::generate_object_begin(""sv, tmp);
        left      = nanojsoncpp::generate_value("val1"sv, true, left);
        left      = nanojsoncpp::generate_value("val2"sv, "string"sv, left);
        left      = nanojsoncpp::generate_object_end(left);

        size_t json_size = nanojsoncpp::generate_finalize(tmp, left);

        std::string_view expected = R"({"val1":true,"val2":"string"})";

        CHECK_EQ(json_size, expected.size());
        CHECK_EQ(std::string_view(tmp, json_size), expected);
    }
}
