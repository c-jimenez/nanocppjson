/*
 * Copyright (c) 2025 c-jimenez
 * SPDX-License-Identifier: MIT
 */

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>

#include <nanojsoncpp/nanojsoncpp.h>

// Compute the total nesting levels of a json object or array tree
static int compute_nesting_level(const nanojsoncpp::json_value& value, int current_level);

TEST_SUITE("json parsing test suite")
{
    TEST_CASE("Null value")
    {
        std::array<nanojsoncpp::json_value_desc, 5u> descriptors;
        nanojsoncpp::json_size_t                     error_index = 0u;
        nanojsoncpp::json_parse_error                error       = nanojsoncpp::json_parse_error::no_error;

        std::string json_input_obj1 = R"({ "test_null1": null })";

        auto root1 = nanojsoncpp::parse(json_input_obj1, descriptors);
        REQUIRE(root1);

        auto null1 = root1->get("test_null1");
        REQUIRE(null1);
        CHECK_EQ(null1->type(), nanojsoncpp::json_value_type::null);
        CHECK_EQ(null1->get(), "null");
        CHECK_FALSE(null1->get("test_null1"));
        CHECK_FALSE(null1->get(0u));

        CHECK(null1->is_null());
        CHECK_FALSE(null1->is_bool());
        CHECK_FALSE(null1->is_string());
        CHECK_FALSE(null1->is_integer());
        CHECK_FALSE(null1->is_decimal());
        CHECK_FALSE(null1->is_array());
        CHECK_FALSE(null1->is_object());

        size_t count = 0u;
        for ([[maybe_unused]] const auto& value : null1.value())
        {
            count++;
        }
        CHECK_EQ(count, 0u);

        std::string json_input_obj2 = R"({ "test_null2": nul })";

        auto root2 = nanojsoncpp::parse(json_input_obj2,
                                        descriptors,
                                        [&](nanojsoncpp::json_size_t _error_index, nanojsoncpp::json_parse_error _error)
                                        {
                                            error_index = _error_index;
                                            error       = _error;
                                        });
        CHECK_FALSE(root2);
        CHECK_EQ(error_index, 16u);
        CHECK_EQ(error, nanojsoncpp::json_parse_error::invalid_value);

        std::string json_input_obj3 = R"({ "test_null3": Null })";

        error_index = 0u;
        error       = nanojsoncpp::json_parse_error::no_error;
        auto root3  = nanojsoncpp::parse(json_input_obj3,
                                        descriptors,
                                        [&](nanojsoncpp::json_size_t _error_index, nanojsoncpp::json_parse_error _error)
                                        {
                                            error_index = _error_index;
                                            error       = _error;
                                        });
        CHECK_FALSE(root3);
        CHECK_EQ(error_index, 16u);
        CHECK_EQ(error, nanojsoncpp::json_parse_error::invalid_value);

        std::string json_input_obj4 = R"({ "test_null4": nul)";

        error_index = 0u;
        error       = nanojsoncpp::json_parse_error::no_error;
        auto root4  = nanojsoncpp::parse(json_input_obj4,
                                        descriptors,
                                        [&](nanojsoncpp::json_size_t _error_index, nanojsoncpp::json_parse_error _error)
                                        {
                                            error_index = _error_index;
                                            error       = _error;
                                        });
        CHECK_FALSE(root4);
        CHECK_EQ(error_index, 16u);
        CHECK_EQ(error, nanojsoncpp::json_parse_error::invalid_value);
    }

    TEST_CASE("Boolean value")
    {
        std::array<nanojsoncpp::json_value_desc, 5u> descriptors;
        nanojsoncpp::json_size_t                     error_index = 0u;
        nanojsoncpp::json_parse_error                error       = nanojsoncpp::json_parse_error::no_error;

        std::string json_input_obj1 = R"({ "test_bool1": true })";

        auto root1 = nanojsoncpp::parse(json_input_obj1, descriptors);
        REQUIRE(root1);

        auto bool1 = root1.value()["test_bool1"];
        REQUIRE(bool1);
        CHECK_EQ(bool1->type(), nanojsoncpp::json_value_type::boolean);
        CHECK_EQ(bool1->get(), "true");
        CHECK(bool1->get<bool>());
        CHECK_FALSE(bool1->get("test_bool1"));
        CHECK_FALSE(bool1->get(0u));

        CHECK_FALSE(bool1->is_null());
        CHECK(bool1->is_bool());
        CHECK_FALSE(bool1->is_string());
        CHECK_FALSE(bool1->is_integer());
        CHECK_FALSE(bool1->is_decimal());
        CHECK_FALSE(bool1->is_array());
        CHECK_FALSE(bool1->is_object());

        size_t count = 0u;
        for ([[maybe_unused]] const auto& value : bool1.value())
        {
            count++;
        }
        CHECK_EQ(count, 0u);

        std::string json_input_obj2 = R"({ "test_bool2": false })";

        auto root2 = nanojsoncpp::parse(json_input_obj2, descriptors);
        REQUIRE(root2);

        auto bool2 = root2->get("test_bool2");
        REQUIRE(bool2);
        CHECK_EQ(bool2->type(), nanojsoncpp::json_value_type::boolean);
        CHECK_EQ(bool2->get(), "false");
        CHECK_FALSE(bool2->get<bool>());

        std::string json_input_obj3 = R"({ "test_bool3": True })";

        auto root3 = nanojsoncpp::parse(json_input_obj3,
                                        descriptors,
                                        [&](nanojsoncpp::json_size_t _error_index, nanojsoncpp::json_parse_error _error)
                                        {
                                            error_index = _error_index;
                                            error       = _error;
                                        });
        CHECK_FALSE(root3);
        CHECK_EQ(error_index, 16u);
        CHECK_EQ(error, nanojsoncpp::json_parse_error::invalid_value);

        std::string json_input_obj4 = R"({ "test_bool4": False })";

        error_index = 0u;
        error       = nanojsoncpp::json_parse_error::no_error;
        auto root4  = nanojsoncpp::parse(json_input_obj4,
                                        descriptors,
                                        [&](nanojsoncpp::json_size_t _error_index, nanojsoncpp::json_parse_error _error)
                                        {
                                            error_index = _error_index;
                                            error       = _error;
                                        });
        CHECK_FALSE(root4);
        CHECK_EQ(error_index, 16u);
        CHECK_EQ(error, nanojsoncpp::json_parse_error::invalid_value);
    }

    TEST_CASE("String value")
    {
        std::array<nanojsoncpp::json_value_desc, 5u> descriptors;
        nanojsoncpp::json_size_t                     error_index = 0u;
        nanojsoncpp::json_parse_error                error       = nanojsoncpp::json_parse_error::no_error;

        std::string json_input_obj1 = R"({ "test_string1": "" })";

        auto root1 = nanojsoncpp::parse(json_input_obj1, descriptors);
        REQUIRE(root1);

        auto string1 = root1->get("test_string1");
        REQUIRE(string1);
        CHECK_EQ(string1->type(), nanojsoncpp::json_value_type::string);
        CHECK_EQ(string1->get(), "");
        CHECK_FALSE(string1->get("test_string1"));
        CHECK_FALSE(string1->get(0u));

        CHECK_FALSE(string1->is_null());
        CHECK_FALSE(string1->is_bool());
        CHECK(string1->is_string());
        CHECK_FALSE(string1->is_integer());
        CHECK_FALSE(string1->is_decimal());
        CHECK_FALSE(string1->is_array());
        CHECK_FALSE(string1->is_object());

        size_t count = 0u;
        for ([[maybe_unused]] const auto& value : string1.value())
        {
            count++;
        }
        CHECK_EQ(count, 0u);

        std::string json_input_obj2 = R"({ "test_string2": "\"" })";

        auto root2 = nanojsoncpp::parse(json_input_obj2, descriptors);
        REQUIRE(root2);

        auto string2 = root2->get("test_string2");
        REQUIRE(string2);
        CHECK_EQ(string2->type(), nanojsoncpp::json_value_type::string);
        CHECK_EQ(string2->get(), std::string_view(R"(\")"));

        std::string json_input_obj3 = R"({ "test_string3": "\"\\\n\tabcedefghijklmnopqrstuvwxyz\/0123456789-\r\b\f" })";

        auto root3 = nanojsoncpp::parse(json_input_obj3, descriptors);
        REQUIRE(root3);

        auto string3 = root3->get("test_string3");
        REQUIRE(string3);
        CHECK_EQ(string3->type(), nanojsoncpp::json_value_type::string);
        CHECK_EQ(string3->get(), std::string_view(R"(\"\\\n\tabcedefghijklmnopqrstuvwxyz\/0123456789-\r\b\f)"));

        char             tmp1[53u];
        std::string_view string3_escaped1 = string3->get_escaped(tmp1);
        CHECK(string3_escaped1.empty());

        char             tmp2[54u];
        std::string_view string3_escaped2 = string3->get_escaped(tmp2);
        CHECK_EQ(string3_escaped2, "\"\\\n\tabcedefghijklmnopqrstuvwxyz/0123456789-\r\b\f");

        std::string_view string3_escape3 = string3->get_escaped();
        CHECK_EQ(string3_escape3, string3_escaped2);
        std::string_view string3_base = string3->get().substr(0u, string3_escape3.size());
        CHECK_EQ(string3_escape3, string3_base);

        std::string json_input_obj4 = R"({ "test_string4": "Not a valid escape sequence : \g..." })";

        auto root4 = nanojsoncpp::parse(json_input_obj4,
                                        descriptors,
                                        [&](nanojsoncpp::json_size_t _error_index, nanojsoncpp::json_parse_error _error)
                                        {
                                            error_index = _error_index;
                                            error       = _error;
                                        });
        CHECK_FALSE(root4);
        CHECK_EQ(error_index, 49u);
        CHECK_EQ(error, nanojsoncpp::json_parse_error::invalid_escaped_char);

        std::string json_input_obj5 = R"({ "test_string5": ")";

        error_index = 0u;
        error       = nanojsoncpp::json_parse_error::no_error;
        auto root5  = nanojsoncpp::parse(json_input_obj5,
                                        descriptors,
                                        [&](nanojsoncpp::json_size_t _error_index, nanojsoncpp::json_parse_error _error)
                                        {
                                            error_index = _error_index;
                                            error       = _error;
                                        });
        CHECK_FALSE(root5);
        CHECK_EQ(error_index, 18u);
        CHECK_EQ(error, nanojsoncpp::json_parse_error::missing_end_of_string);

        std::string json_input_obj6 = R"({ "test_string6": "Missing end of string })";

        error_index = 0u;
        error       = nanojsoncpp::json_parse_error::no_error;
        auto root6  = nanojsoncpp::parse(json_input_obj6,
                                        descriptors,
                                        [&](nanojsoncpp::json_size_t _error_index, nanojsoncpp::json_parse_error _error)
                                        {
                                            error_index = _error_index;
                                            error       = _error;
                                        });
        CHECK_FALSE(root6);
        CHECK_EQ(error_index, 19u);
        CHECK_EQ(error, nanojsoncpp::json_parse_error::missing_end_of_string);

        std::string json_input_obj7 = R"({ "test_string7": "Missing end of string \)";

        error_index = 0u;
        error       = nanojsoncpp::json_parse_error::no_error;
        auto root7  = nanojsoncpp::parse(json_input_obj7,
                                        descriptors,
                                        [&](nanojsoncpp::json_size_t _error_index, nanojsoncpp::json_parse_error _error)
                                        {
                                            error_index = _error_index;
                                            error       = _error;
                                        });
        CHECK_FALSE(root7);
        CHECK_EQ(error_index, 41u);
        CHECK_EQ(error, nanojsoncpp::json_parse_error::missing_end_of_string);
    }

    TEST_CASE("Integer value")
    {
        std::array<nanojsoncpp::json_value_desc, 5u> descriptors;
        nanojsoncpp::json_size_t                     error_index = 0u;
        nanojsoncpp::json_parse_error                error       = nanojsoncpp::json_parse_error::no_error;

        std::string json_input_obj1 = R"({ "test_int1": 1234567890 })";

        auto root1 = nanojsoncpp::parse(json_input_obj1, descriptors);
        REQUIRE(root1);

        auto int1 = root1->get("test_int1");
        REQUIRE(int1);
        CHECK_EQ(int1->type(), nanojsoncpp::json_value_type::integer);
        CHECK_EQ(int1->get(), "1234567890");
        CHECK_EQ(int1->get<int>(), 1234567890);
        CHECK_FALSE(int1->get("test_int1"));
        CHECK_FALSE(int1->get(0u));

        CHECK_FALSE(int1->is_null());
        CHECK_FALSE(int1->is_bool());
        CHECK_FALSE(int1->is_string());
        CHECK(int1->is_integer());
        CHECK(int1->is_decimal());
        CHECK_FALSE(int1->is_array());
        CHECK_FALSE(int1->is_object());

        size_t count = 0u;
        for ([[maybe_unused]] const auto& value : int1.value())
        {
            count++;
        }
        CHECK_EQ(count, 0u);

        std::string json_input_obj2 = R"({ "test_int2": -1234567890 })";

        auto root2 = nanojsoncpp::parse(json_input_obj2, descriptors);
        REQUIRE(root2);

        auto int2 = root2->get("test_int2");
        REQUIRE(int2);
        CHECK_EQ(int2->type(), nanojsoncpp::json_value_type::integer);
        CHECK_EQ(int2->get(), "-1234567890");
        CHECK_EQ(int2->get<int>(), -1234567890);

        std::string json_input_obj3 = R"({ "test_int3": +1234567890 })";

        auto root3 = nanojsoncpp::parse(json_input_obj3,
                                        descriptors,
                                        [&](nanojsoncpp::json_size_t _error_index, nanojsoncpp::json_parse_error _error)
                                        {
                                            error_index = _error_index;
                                            error       = _error;
                                        });
        CHECK_FALSE(root3);
        CHECK_EQ(error_index, 15u);
        CHECK_EQ(error, nanojsoncpp::json_parse_error::invalid_value);

        std::string json_input_obj4 = R"({ "test_int4": 1234a567890 })";

        error_index = 0u;
        error       = nanojsoncpp::json_parse_error::no_error;
        auto root4  = nanojsoncpp::parse(json_input_obj4,
                                        descriptors,
                                        [&](nanojsoncpp::json_size_t _error_index, nanojsoncpp::json_parse_error _error)
                                        {
                                            error_index = _error_index;
                                            error       = _error;
                                        });
        CHECK_FALSE(root4);
        CHECK_EQ(error_index, 20u);
        CHECK_EQ(error, nanojsoncpp::json_parse_error::unexpected_char);
    }

    TEST_CASE("Decimal value")
    {
        std::array<nanojsoncpp::json_value_desc, 5u> descriptors;
        nanojsoncpp::json_size_t                     error_index = 0u;
        nanojsoncpp::json_parse_error                error       = nanojsoncpp::json_parse_error::no_error;

        std::string json_input_obj1 = R"({ "test_dec1": 12345.6789 })";

        auto root1 = nanojsoncpp::parse(json_input_obj1, descriptors);
        REQUIRE(root1);

        auto dec1 = root1->get("test_dec1");
        REQUIRE(dec1);
        CHECK_EQ(dec1->type(), nanojsoncpp::json_value_type::decimal);
        CHECK_EQ(dec1->get(), "12345.6789");
        CHECK_EQ(dec1->get<nanojsoncpp::json_float_t>(), static_cast<nanojsoncpp::json_float_t>(12345.6789));
        CHECK_FALSE(dec1->get("test_dec1"));
        CHECK_FALSE(dec1->get(0u));

        CHECK_FALSE(dec1->is_null());
        CHECK_FALSE(dec1->is_bool());
        CHECK_FALSE(dec1->is_string());
        CHECK_FALSE(dec1->is_integer());
        CHECK(dec1->is_decimal());
        CHECK_FALSE(dec1->is_array());
        CHECK_FALSE(dec1->is_object());

        size_t count = 0u;
        for ([[maybe_unused]] const auto& value : dec1.value())
        {
            count++;
        }
        CHECK_EQ(count, 0u);

        std::string json_input_obj2 = R"({ "test_dec2": -12345.6789 })";

        auto root2 = nanojsoncpp::parse(json_input_obj2, descriptors);
        REQUIRE(root2);

        auto dec2 = root2->get("test_dec2");
        REQUIRE(dec2);
        CHECK_EQ(dec2->type(), nanojsoncpp::json_value_type::decimal);
        CHECK_EQ(dec2->get(), "-12345.6789");
        CHECK_EQ(dec2->get<nanojsoncpp::json_float_t>(), static_cast<nanojsoncpp::json_float_t>(-12345.6789));

        std::string json_input_obj3 = R"({ "test_dec3": +12345.6789 })";

        auto root3 = nanojsoncpp::parse(json_input_obj3,
                                        descriptors,
                                        [&](nanojsoncpp::json_size_t _error_index, nanojsoncpp::json_parse_error _error)
                                        {
                                            error_index = _error_index;
                                            error       = _error;
                                        });
        CHECK_FALSE(root3);
        CHECK_EQ(error_index, 15u);
        CHECK_EQ(error, nanojsoncpp::json_parse_error::invalid_value);

        std::string json_input_obj4 = R"({ "test_dec4": .123456789 })";

        error_index = 0u;
        error       = nanojsoncpp::json_parse_error::no_error;
        auto root4  = nanojsoncpp::parse(json_input_obj4,
                                        descriptors,
                                        [&](nanojsoncpp::json_size_t _error_index, nanojsoncpp::json_parse_error _error)
                                        {
                                            error_index = _error_index;
                                            error       = _error;
                                        });
        CHECK_FALSE(root4);
        CHECK_EQ(error_index, 15u);
        CHECK_EQ(error, nanojsoncpp::json_parse_error::invalid_value);

        std::string json_input_obj5 = R"({ "test_dec5": 0.123456.789 })";

        error_index = 0u;
        error       = nanojsoncpp::json_parse_error::no_error;
        auto root5  = nanojsoncpp::parse(json_input_obj5,
                                        descriptors,
                                        [&](nanojsoncpp::json_size_t _error_index, nanojsoncpp::json_parse_error _error)
                                        {
                                            error_index = _error_index;
                                            error       = _error;
                                        });
        CHECK_FALSE(root5);
        CHECK_EQ(error_index, 15u);
        CHECK_EQ(error, nanojsoncpp::json_parse_error::invalid_value);

        std::string json_input_obj6 = R"({ "test_dec6": -1234.56-789 })";

        error_index = 0u;
        error       = nanojsoncpp::json_parse_error::no_error;
        auto root6  = nanojsoncpp::parse(json_input_obj6,
                                        descriptors,
                                        [&](nanojsoncpp::json_size_t _error_index, nanojsoncpp::json_parse_error _error)
                                        {
                                            error_index = _error_index;
                                            error       = _error;
                                        });
        CHECK_FALSE(root6);
        CHECK_EQ(error_index, 15u);
        CHECK_EQ(error, nanojsoncpp::json_parse_error::invalid_value);

        std::string json_input_obj7 = R"({ "test_dec7": - })";

        error_index = 0u;
        error       = nanojsoncpp::json_parse_error::no_error;
        auto root7  = nanojsoncpp::parse(json_input_obj7,
                                        descriptors,
                                        [&](nanojsoncpp::json_size_t _error_index, nanojsoncpp::json_parse_error _error)
                                        {
                                            error_index = _error_index;
                                            error       = _error;
                                        });
        CHECK_FALSE(root7);
        CHECK_EQ(error_index, 15u);
        CHECK_EQ(error, nanojsoncpp::json_parse_error::invalid_value);

        std::string json_input_obj8 = R"({ "test_dec8": 0.123456)";

        error_index = 0u;
        error       = nanojsoncpp::json_parse_error::no_error;
        auto root8  = nanojsoncpp::parse(json_input_obj8,
                                        descriptors,
                                        [&](nanojsoncpp::json_size_t _error_index, nanojsoncpp::json_parse_error _error)
                                        {
                                            error_index = _error_index;
                                            error       = _error;
                                        });
        CHECK_FALSE(root8);
        CHECK_EQ(error_index, 15u);
        CHECK_EQ(error, nanojsoncpp::json_parse_error::invalid_value);

        std::string json_input_obj9 = R"({ "test_dec8": 0.1234567s89 })";

        error_index = 0u;
        error       = nanojsoncpp::json_parse_error::no_error;
        auto root9  = nanojsoncpp::parse(json_input_obj9,
                                        descriptors,
                                        [&](nanojsoncpp::json_size_t _error_index, nanojsoncpp::json_parse_error _error)
                                        {
                                            error_index = _error_index;
                                            error       = _error;
                                        });
        CHECK_FALSE(root9);
        CHECK_EQ(error_index, 25u);
        CHECK_EQ(error, nanojsoncpp::json_parse_error::unexpected_char);
    }

    TEST_CASE("Object value")
    {
        std::array<nanojsoncpp::json_value_desc, 10u> descriptors;
        nanojsoncpp::json_size_t                      error_index = 0u;
        nanojsoncpp::json_parse_error                 error       = nanojsoncpp::json_parse_error::no_error;

        std::string json_input_obj0 = R"(
            {

            }
            )";

        auto root0 = nanojsoncpp::parse(json_input_obj0, descriptors);
        REQUIRE(root0);
        CHECK_EQ(root0->name(), "");
        CHECK_EQ(root0->type(), nanojsoncpp::json_value_type::object);
        CHECK_EQ(root0->size(), 0u);

        CHECK_FALSE(root0->is_null());
        CHECK_FALSE(root0->is_bool());
        CHECK_FALSE(root0->is_string());
        CHECK_FALSE(root0->is_integer());
        CHECK_FALSE(root0->is_decimal());
        CHECK_FALSE(root0->is_array());
        CHECK(root0->is_object());

        size_t count = 0;
        for ([[maybe_unused]] const auto& value : root0.value())
        {
            count++;
        }
        CHECK_EQ(count, 0u);

        std::string json_input_obj1 = R"(
        {
            "test_null": null,"test_bool":true

            ,"test_string": "My string!\n"

            , "test_int": 1234,"test_decimal": -1.23456
        }
        )";

        auto root1 = nanojsoncpp::parse(json_input_obj1, descriptors);
        REQUIRE(root1);
        CHECK_EQ(root1->name(), "");
        CHECK_EQ(root1->type(), nanojsoncpp::json_value_type::object);
        CHECK_EQ(root1->size(), 5u);
        CHECK_FALSE(root1->get(0u));

        auto null1 = root1->get("test_null");
        REQUIRE(null1);
        CHECK_EQ(null1->name(), "test_null");
        CHECK_EQ(null1->type(), nanojsoncpp::json_value_type::null);
        CHECK_EQ(null1->get(), "null");

        auto bool1 = root1->get("test_bool");
        REQUIRE(bool1);
        CHECK_EQ(bool1->name(), "test_bool");
        CHECK_EQ(bool1->type(), nanojsoncpp::json_value_type::boolean);
        CHECK_EQ(bool1->get(), "true");
        CHECK_EQ(bool1->get<bool>(), true);

        auto string1 = root1->get("test_string");
        REQUIRE(string1);
        CHECK_EQ(string1->name(), "test_string");
        CHECK_EQ(string1->type(), nanojsoncpp::json_value_type::string);
        CHECK_EQ(string1->get(), "My string!\\n");

        auto int1 = root1->get("test_int");
        REQUIRE(int1);
        CHECK_EQ(int1->name(), "test_int");
        CHECK_EQ(int1->type(), nanojsoncpp::json_value_type::integer);
        CHECK_EQ(int1->get(), "1234");
        CHECK_EQ(int1->get<unsigned int>(), 1234);

        auto dec1 = root1->get("test_decimal");
        REQUIRE(dec1);
        CHECK_EQ(dec1->name(), "test_decimal");
        CHECK_EQ(dec1->type(), nanojsoncpp::json_value_type::decimal);
        CHECK_EQ(dec1->get(), "-1.23456");
        CHECK_EQ(dec1->get<float>(), -1.23456f);

        static const std::array<nanojsoncpp::json_value_type, 5u> expected_types = {nanojsoncpp::json_value_type::null,
                                                                                    nanojsoncpp::json_value_type::boolean,
                                                                                    nanojsoncpp::json_value_type::string,
                                                                                    nanojsoncpp::json_value_type::integer,
                                                                                    nanojsoncpp::json_value_type::decimal};

        size_t index = 0u;
        for (const nanojsoncpp::json_value& value : root1.value())
        {
            CHECK_EQ(value.type(), expected_types[index]);
            index++;
        }

        std::string json_input_obj2 = R"(
            {
                "test_null": null,"test_bool":true
    
                ,"test_string": "My string!\n"
    
                , "test_int": 1234,"test_decimal": -1.23456
            ]
            )";

        auto root2 = nanojsoncpp::parse(json_input_obj2,
                                        descriptors,
                                        [&](nanojsoncpp::json_size_t _error_index, nanojsoncpp::json_parse_error _error)
                                        {
                                            error_index = _error_index;
                                            error       = _error;
                                        });
        CHECK_FALSE(root2);
        CHECK_EQ(error_index, 196u);
        CHECK_EQ(error, nanojsoncpp::json_parse_error::unexpected_char);

        std::string json_input_obj3 = R"(
            {
                "test_null" null,"test_bool":true
    
                ,"test_string": "My string!\n"
    
                , "test_int": 1234,"test_decimal": -1.23456
            }
            )";

        auto root3 = nanojsoncpp::parse(json_input_obj3,
                                        descriptors,
                                        [&](nanojsoncpp::json_size_t _error_index, nanojsoncpp::json_parse_error _error)
                                        {
                                            error_index = _error_index;
                                            error       = _error;
                                        });
        CHECK_FALSE(root3);
        CHECK_EQ(error_index, 44u);
        CHECK_EQ(error, nanojsoncpp::json_parse_error::missing_value_separator);

        std::string json_input_obj4 = R"(
            {
                "test_null": null,
                "test_bool:true
            }
            )";

        auto root4 = nanojsoncpp::parse(json_input_obj4,
                                        descriptors,
                                        [&](nanojsoncpp::json_size_t _error_index, nanojsoncpp::json_parse_error _error)
                                        {
                                            error_index = _error_index;
                                            error       = _error;
                                        });
        CHECK_FALSE(root4);
        CHECK_EQ(error_index, 67u);
        CHECK_EQ(error, nanojsoncpp::json_parse_error::missing_end_of_string);

        std::string json_input_obj5 = R"(
            {
                "test_null": null,
                test_bool":true
            }
            )";

        auto root5 = nanojsoncpp::parse(json_input_obj5,
                                        descriptors,
                                        [&](nanojsoncpp::json_size_t _error_index, nanojsoncpp::json_parse_error _error)
                                        {
                                            error_index = _error_index;
                                            error       = _error;
                                        });
        CHECK_FALSE(root5);
        CHECK_EQ(error_index, 67u);
        CHECK_EQ(error, nanojsoncpp::json_parse_error::unexpected_char);

        std::string json_input_obj6 = R"(
            {
                "test_null": null,
                "test_bool":true,
            }
            )";

        auto root6 = nanojsoncpp::parse(json_input_obj6,
                                        descriptors,
                                        [&](nanojsoncpp::json_size_t _error_index, nanojsoncpp::json_parse_error _error)
                                        {
                                            error_index = _error_index;
                                            error       = _error;
                                        });
        CHECK_FALSE(root6);
        CHECK_EQ(error_index, 97u);
        CHECK_EQ(error, nanojsoncpp::json_parse_error::expected_member_name);

        std::string json_input_obj7 = R"(
            {
                "test_null": null,
                "test_bool":],
            }
            )";

        auto root7 = nanojsoncpp::parse(json_input_obj7,
                                        descriptors,
                                        [&](nanojsoncpp::json_size_t _error_index, nanojsoncpp::json_parse_error _error)
                                        {
                                            error_index = _error_index;
                                            error       = _error;
                                        });
        CHECK_FALSE(root7);
        CHECK_EQ(error_index, 78u);
        CHECK_EQ(error, nanojsoncpp::json_parse_error::unexpected_char);
    }

    TEST_CASE("Array value")
    {
        std::array<nanojsoncpp::json_value_desc, 10u> descriptors;
        nanojsoncpp::json_size_t                      error_index = 0u;
        nanojsoncpp::json_parse_error                 error       = nanojsoncpp::json_parse_error::no_error;

        std::string json_input_obj0 = R"(
            [

            ]
            )";

        auto root0 = nanojsoncpp::parse(json_input_obj0, descriptors);
        REQUIRE(root0);
        CHECK_EQ(root0->name(), "");
        CHECK_EQ(root0->type(), nanojsoncpp::json_value_type::array);
        CHECK_EQ(root0->size(), 0u);
        CHECK_FALSE(root0->get(0u));
        CHECK_FALSE(root0.value()[0u]);

        CHECK_FALSE(root0->is_null());
        CHECK_FALSE(root0->is_bool());
        CHECK_FALSE(root0->is_string());
        CHECK_FALSE(root0->is_integer());
        CHECK_FALSE(root0->is_decimal());
        CHECK(root0->is_array());
        CHECK_FALSE(root0->is_object());

        size_t count = 0;
        for ([[maybe_unused]] const auto& value : root0.value())
        {
            count++;
        }
        CHECK_EQ(count, 0u);

        std::string json_input_obj1 = R"(
            [
                null, null,
                null
            ]
            )";

        auto root1 = nanojsoncpp::parse(json_input_obj1, descriptors);
        REQUIRE(root1);
        CHECK_EQ(root1->name(), "");
        CHECK_EQ(root1->type(), nanojsoncpp::json_value_type::array);
        CHECK_EQ(root1->size(), 3u);

        for (const auto& value : root1.value())
        {
            CHECK_EQ(value.type(), nanojsoncpp::json_value_type::null);
            CHECK_EQ(value.name(), "");
        }

        std::string json_input_obj2 = R"(
        [
            true, false,
            false, true
        ]
        )";

        auto root2 = nanojsoncpp::parse(json_input_obj2, descriptors);
        REQUIRE(root2);
        CHECK_EQ(root2->name(), "");
        CHECK_EQ(root2->type(), nanojsoncpp::json_value_type::array);
        CHECK_EQ(root2->size(), 4u);

        static const std::array<bool, 4u> expected_bool_values = {true, false, false, true};
        size_t                            index                = 0u;
        for (const auto& value : root2.value())
        {
            CHECK_EQ(value.type(), nanojsoncpp::json_value_type::boolean);
            CHECK_EQ(value.name(), "");
            CHECK_EQ(value.get<bool>(), expected_bool_values[index]);
            index++;
        }

        auto bool1 = root2->get(0u);
        REQUIRE(bool1);
        CHECK_EQ(bool1->get<bool>(), true);

        auto bool2 = root2->get(1u);
        REQUIRE(bool2);
        CHECK_EQ(bool2->get<bool>(), false);

        auto bool3 = root2->get(2u);
        REQUIRE(bool3);
        CHECK_EQ(bool3->get<bool>(), false);

        auto bool4 = root2->get(3u);
        REQUIRE(bool4);
        CHECK_EQ(bool4->get<bool>(), true);

        CHECK_FALSE(root2->get(4u));
        CHECK_FALSE(root2.value()[4u]);

        std::string json_input_obj3 = R"(
            [
                "string1", "string2",
                "string3"
            ]
            )";

        auto root3 = nanojsoncpp::parse(json_input_obj3, descriptors);
        REQUIRE(root3);
        CHECK_EQ(root3->name(), "");
        CHECK_EQ(root3->type(), nanojsoncpp::json_value_type::array);
        CHECK_EQ(root3->size(), 3u);

        static const std::array<std::string, 3u> expected_string_values = {"string1", "string2", "string3"};
        index                                                           = 0u;
        for (const auto& value : root3.value())
        {
            CHECK_EQ(value.type(), nanojsoncpp::json_value_type::string);
            CHECK_EQ(value.name(), "");
            CHECK_EQ(value.get(), expected_string_values[index]);
            index++;
        }

        std::string json_input_obj4 = R"(
            [
                123456789, -1234.5678,
                -1234567890, 12345.678
            ]
            )";

        auto root4 = nanojsoncpp::parse(json_input_obj4, descriptors);
        REQUIRE(root4);
        CHECK_EQ(root4->name(), "");
        CHECK_EQ(root4->type(), nanojsoncpp::json_value_type::array);
        CHECK_EQ(root4->size(), 4u);

        static const std::array<nanojsoncpp::json_value_type, 4u> expected_float_types = {nanojsoncpp::json_value_type::integer,
                                                                                          nanojsoncpp::json_value_type::decimal,
                                                                                          nanojsoncpp::json_value_type::integer,
                                                                                          nanojsoncpp::json_value_type::decimal};
        static const std::array<nanojsoncpp::json_float_t, 4u> expected_float_values = {static_cast<nanojsoncpp::json_float_t>(123456789.),
                                                                                        static_cast<nanojsoncpp::json_float_t>(-1234.5678),
                                                                                        static_cast<nanojsoncpp::json_float_t>(-1234567890),
                                                                                        static_cast<nanojsoncpp::json_float_t>(12345.678)};
        index                                                                        = 0u;
        for (const auto& value : root4.value())
        {
            CHECK_EQ(value.type(), expected_float_types[index]);
            CHECK_EQ(value.name(), "");
            CHECK_EQ(value.get<nanojsoncpp::json_float_t>(), expected_float_values[index]);
            index++;
        }

        std::string json_input_obj5 = R"(
            [
                null, true, "my_string",
                -1234567890, 12345.678
            ]
            )";

        auto root5 = nanojsoncpp::parse(json_input_obj5, descriptors);
        REQUIRE(root5);
        CHECK_EQ(root5->name(), "");
        CHECK_EQ(root5->type(), nanojsoncpp::json_value_type::array);
        CHECK_EQ(root5->size(), 5u);

        static const std::array<nanojsoncpp::json_value_type, 5u> expected_values_types = {nanojsoncpp::json_value_type::null,
                                                                                           nanojsoncpp::json_value_type::boolean,
                                                                                           nanojsoncpp::json_value_type::string,
                                                                                           nanojsoncpp::json_value_type::integer,
                                                                                           nanojsoncpp::json_value_type::decimal};
        index                                                                           = 0u;
        for (const auto& value : root4.value())
        {
            CHECK_EQ(value.type(), expected_values_types[index]);
            CHECK_EQ(value.name(), "");
            index++;
        }

        std::string json_input_obj6 = R"(
            [
                true, false,
                true,
            ]
            )";

        auto root6 = nanojsoncpp::parse(json_input_obj6,
                                        descriptors,
                                        [&](nanojsoncpp::json_size_t _error_index, nanojsoncpp::json_parse_error _error)
                                        {
                                            error_index = _error_index;
                                            error       = _error;
                                        });
        CHECK_FALSE(root6);
        CHECK_EQ(error_index, 91u);
        CHECK_EQ(error, nanojsoncpp::json_parse_error::unexpected_char);

        std::string json_input_obj7 = R"(
            [
                true, false,
                true
            }
            )";

        auto root7 = nanojsoncpp::parse(json_input_obj7,
                                        descriptors,
                                        [&](nanojsoncpp::json_size_t _error_index, nanojsoncpp::json_parse_error _error)
                                        {
                                            error_index = _error_index;
                                            error       = _error;
                                        });
        CHECK_FALSE(root7);
        CHECK_EQ(error_index, 78u);
        CHECK_EQ(error, nanojsoncpp::json_parse_error::unexpected_char);
    }

    TEST_CASE("Nested values")
    {
        std::array<nanojsoncpp::json_value_desc, 1000u> descriptors;

        SUBCASE("Objects")
        {
            std::string json_input_obj1 = R"(
            {
                "test_object1": 
                {
                    "test_null": null,
                    "test_subobject1": {
                        "test_val1": 9876,
                        "test_val2": null
                    },
                    "test_bool":true
                },
                "test_object2":{},
                "test_object3":
                {
                    "test_string": "My string!\n"
                    , "test_int": 1234,"test_decimal": -1.23456
                }
            }
            )";

            auto root1 = nanojsoncpp::parse(json_input_obj1, descriptors);
            REQUIRE(root1);
            CHECK_EQ(root1->name(), "");
            CHECK_EQ(root1->type(), nanojsoncpp::json_value_type::object);
            CHECK_EQ(root1->size(), 3u);

            auto object1 = root1->get("test_object1");
            REQUIRE(object1);
            CHECK_EQ(object1->name(), "test_object1");
            CHECK_EQ(object1->type(), nanojsoncpp::json_value_type::object);
            CHECK_EQ(object1->size(), 3u);

            auto object2 = root1->get("test_object2");
            REQUIRE(object2);
            CHECK_EQ(object2->name(), "test_object2");
            CHECK_EQ(object2->type(), nanojsoncpp::json_value_type::object);
            CHECK_EQ(object2->size(), 0u);

            auto object3 = root1->get("test_object3");
            REQUIRE(object3);
            CHECK_EQ(object3->name(), "test_object3");
            CHECK_EQ(object3->type(), nanojsoncpp::json_value_type::object);
            CHECK_EQ(object3->size(), 3u);

            auto null1 = object1->get("test_null");
            REQUIRE(null1);
            CHECK_EQ(null1->name(), "test_null");
            CHECK_EQ(null1->type(), nanojsoncpp::json_value_type::null);
            CHECK_EQ(null1->get(), "null");

            auto subobject1 = object1->get("test_subobject1");
            REQUIRE(subobject1);
            CHECK_EQ(subobject1->name(), "test_subobject1");
            CHECK_EQ(subobject1->type(), nanojsoncpp::json_value_type::object);
            CHECK_EQ(subobject1->size(), 2u);

            auto val1 = subobject1->get("test_val1");
            REQUIRE(val1);
            CHECK_EQ(val1->name(), "test_val1");
            CHECK_EQ(val1->type(), nanojsoncpp::json_value_type::integer);
            CHECK_EQ(val1->get(), "9876");
            CHECK_EQ(val1->get<unsigned int>(), 9876);

            auto val2 = subobject1->get("test_val2");
            REQUIRE(val2);
            CHECK_EQ(val2->name(), "test_val2");
            CHECK_EQ(val2->type(), nanojsoncpp::json_value_type::null);
            CHECK_EQ(val2->get(), "null");

            auto bool1 = object1->get("test_bool");
            REQUIRE(bool1);
            CHECK_EQ(bool1->name(), "test_bool");
            CHECK_EQ(bool1->type(), nanojsoncpp::json_value_type::boolean);
            CHECK_EQ(bool1->get(), "true");
            CHECK_EQ(bool1->get<bool>(), true);

            auto string1 = object3->get("test_string");
            REQUIRE(string1);
            CHECK_EQ(string1->name(), "test_string");
            CHECK_EQ(string1->type(), nanojsoncpp::json_value_type::string);
            CHECK_EQ(string1->get(), "My string!\\n");

            auto int1 = object3->get("test_int");
            REQUIRE(int1);
            CHECK_EQ(int1->name(), "test_int");
            CHECK_EQ(int1->type(), nanojsoncpp::json_value_type::integer);
            CHECK_EQ(int1->get(), "1234");
            CHECK_EQ(int1->get<unsigned int>(), 1234);

            auto dec1 = object3->get("test_decimal");
            REQUIRE(dec1);
            CHECK_EQ(dec1->name(), "test_decimal");
            CHECK_EQ(dec1->type(), nanojsoncpp::json_value_type::decimal);
            CHECK_EQ(dec1->get(), "-1.23456");
            CHECK_EQ(dec1->get<float>(), -1.23456f);

            for (const nanojsoncpp::json_value& value : root1.value())
            {
                CHECK_EQ(value.type(), nanojsoncpp::json_value_type::object);
            }

            static const std::array<nanojsoncpp::json_value_type, 3u> expected_types_object1 = {
                nanojsoncpp::json_value_type::null, nanojsoncpp::json_value_type::object, nanojsoncpp::json_value_type::boolean};

            size_t index = 0u;
            for (const nanojsoncpp::json_value& value : object1.value())
            {
                CHECK_EQ(value.type(), expected_types_object1[index]);
                index++;
            }

            static const std::array<nanojsoncpp::json_value_type, 2u> expected_types_subobject1 = {nanojsoncpp::json_value_type::integer,
                                                                                                   nanojsoncpp::json_value_type::null};

            index = 0u;
            for (const nanojsoncpp::json_value& value : subobject1.value())
            {
                CHECK_EQ(value.type(), expected_types_subobject1[index]);
                index++;
            }

            index = 0u;
            for ([[maybe_unused]] const nanojsoncpp::json_value& value : object2.value())
            {
                index++;
            }
            CHECK_EQ(index, 0u);

            static const std::array<nanojsoncpp::json_value_type, 3u> expected_types_object3 = {
                nanojsoncpp::json_value_type::string, nanojsoncpp::json_value_type::integer, nanojsoncpp::json_value_type::decimal};

            index = 0u;
            for (const nanojsoncpp::json_value& value : object3.value())
            {
                CHECK_EQ(value.type(), expected_types_object3[index]);
                index++;
            }
        }

        SUBCASE("Arrays")
        {
            std::string json_input_obj1 = R"(
            [
                null, true, [ 
                              123, [], [
                                          3.14, "my string"
                                       ]
                            ], 567
            ]
            )";

            auto root1 = nanojsoncpp::parse(json_input_obj1, descriptors);
            REQUIRE(root1);
            CHECK_EQ(root1->name(), "");
            CHECK_EQ(root1->type(), nanojsoncpp::json_value_type::array);
            CHECK_EQ(root1->size(), 4u);

            auto null1 = root1->get(0u);
            REQUIRE(null1);
            CHECK_EQ(null1->name(), "");
            CHECK_EQ(null1->type(), nanojsoncpp::json_value_type::null);
            CHECK_EQ(null1->get(), "null");

            auto bool1 = root1->get(1u);
            REQUIRE(bool1);
            CHECK_EQ(bool1->name(), "");
            CHECK_EQ(bool1->type(), nanojsoncpp::json_value_type::boolean);
            CHECK_EQ(bool1->get(), "true");
            CHECK_EQ(bool1->get<bool>(), true);

            auto subarray1 = root1->get(2u);
            REQUIRE(subarray1);
            CHECK_EQ(subarray1->name(), "");
            CHECK_EQ(subarray1->type(), nanojsoncpp::json_value_type::array);
            CHECK_EQ(subarray1->size(), 3u);

            auto int1 = root1->get(3u);
            REQUIRE(int1);
            CHECK_EQ(int1->name(), "");
            CHECK_EQ(int1->type(), nanojsoncpp::json_value_type::integer);
            CHECK_EQ(int1->get(), "567");
            CHECK_EQ(int1->get<int>(), 567);

            auto int2 = subarray1->get(0u);
            REQUIRE(int2);
            CHECK_EQ(int2->name(), "");
            CHECK_EQ(int2->type(), nanojsoncpp::json_value_type::integer);
            CHECK_EQ(int2->get(), "123");
            CHECK_EQ(int2->get<int>(), 123);

            auto subarray2 = subarray1->get(1u);
            REQUIRE(subarray2);
            CHECK_EQ(subarray2->name(), "");
            CHECK_EQ(subarray2->type(), nanojsoncpp::json_value_type::array);
            CHECK_EQ(subarray2->size(), 0u);

            auto subarray3 = subarray1->get(2u);
            REQUIRE(subarray3);
            CHECK_EQ(subarray3->name(), "");
            CHECK_EQ(subarray3->type(), nanojsoncpp::json_value_type::array);
            CHECK_EQ(subarray3->size(), 2u);

            auto dec1 = subarray3->get(0u);
            REQUIRE(dec1);
            CHECK_EQ(dec1->name(), "");
            CHECK_EQ(dec1->type(), nanojsoncpp::json_value_type::decimal);
            CHECK_EQ(dec1->get(), "3.14");
            CHECK_EQ(dec1->get<nanojsoncpp::json_float_t>(), static_cast<nanojsoncpp::json_float_t>(3.14));

            auto string1 = subarray3->get(1u);
            REQUIRE(string1);
            CHECK_EQ(string1->name(), "");
            CHECK_EQ(string1->type(), nanojsoncpp::json_value_type::string);
            CHECK_EQ(string1->get(), "my string");
        }

        SUBCASE("Mix")
        {
            std::string json_input_obj1 = R"(
            [
                null, true, { 
                              "test_int":123, 
                              "test_null_array": [], 
                              "test_array": [
                                          3.14, {
                                                    "test_int": 456,
                                                    "test_bool": false
                                                }, "my string"
                                       ]
                            }, 567
            ]
            )";

            auto root1 = nanojsoncpp::parse(json_input_obj1, descriptors);
            REQUIRE(root1);
            CHECK_EQ(root1->name(), "");
            CHECK_EQ(root1->type(), nanojsoncpp::json_value_type::array);
            CHECK_EQ(root1->size(), 4u);

            auto null1 = root1->get(0u);
            REQUIRE(null1);
            CHECK_EQ(null1->name(), "");
            CHECK_EQ(null1->type(), nanojsoncpp::json_value_type::null);
            CHECK_EQ(null1->get(), "null");

            auto bool1 = root1->get(1u);
            REQUIRE(bool1);
            CHECK_EQ(bool1->name(), "");
            CHECK_EQ(bool1->type(), nanojsoncpp::json_value_type::boolean);
            CHECK_EQ(bool1->get(), "true");
            CHECK_EQ(bool1->get<bool>(), true);

            auto object1 = root1->get(2u);
            REQUIRE(object1);
            CHECK_EQ(object1->name(), "");
            CHECK_EQ(object1->type(), nanojsoncpp::json_value_type::object);
            CHECK_EQ(object1->size(), 3u);

            auto int1 = root1->get(3u);
            REQUIRE(int1);
            CHECK_EQ(int1->name(), "");
            CHECK_EQ(int1->type(), nanojsoncpp::json_value_type::integer);
            CHECK_EQ(int1->get(), "567");
            CHECK_EQ(int1->get<int>(), 567);

            auto int2 = object1->get("test_int");
            REQUIRE(int2);
            CHECK_EQ(int2->name(), "test_int");
            CHECK_EQ(int2->type(), nanojsoncpp::json_value_type::integer);
            CHECK_EQ(int2->get(), "123");
            CHECK_EQ(int2->get<int>(), 123);

            auto subarray1 = object1->get("test_null_array");
            REQUIRE(subarray1);
            CHECK_EQ(subarray1->name(), "test_null_array");
            CHECK_EQ(subarray1->type(), nanojsoncpp::json_value_type::array);
            CHECK_EQ(subarray1->size(), 0u);

            auto subarray2 = object1->get("test_array");
            REQUIRE(subarray2);
            CHECK_EQ(subarray2->name(), "test_array");
            CHECK_EQ(subarray2->type(), nanojsoncpp::json_value_type::array);
            CHECK_EQ(subarray2->size(), 3u);

            auto dec1 = subarray2->get(0u);
            REQUIRE(dec1);
            CHECK_EQ(dec1->name(), "");
            CHECK_EQ(dec1->type(), nanojsoncpp::json_value_type::decimal);
            CHECK_EQ(dec1->get(), "3.14");
            CHECK_EQ(dec1->get<nanojsoncpp::json_float_t>(), static_cast<nanojsoncpp::json_float_t>(3.14));

            auto object2 = subarray2->get(1u);
            REQUIRE(object2);
            CHECK_EQ(object2->name(), "");
            CHECK_EQ(object2->type(), nanojsoncpp::json_value_type::object);
            CHECK_EQ(object2->size(), 2u);

            auto string1 = subarray2->get(2u);
            REQUIRE(string1);
            CHECK_EQ(string1->name(), "");
            CHECK_EQ(string1->type(), nanojsoncpp::json_value_type::string);
            CHECK_EQ(string1->get(), "my string");

            auto int3 = object2->get("test_int");
            REQUIRE(int3);
            CHECK_EQ(int3->name(), "test_int");
            CHECK_EQ(int3->type(), nanojsoncpp::json_value_type::integer);
            CHECK_EQ(int3->get(), "456");
            CHECK_EQ(int3->get<int>(), 456);

            auto bool2 = object2->get("test_bool");
            REQUIRE(bool2);
            CHECK_EQ(bool2->name(), "test_bool");
            CHECK_EQ(bool2->type(), nanojsoncpp::json_value_type::boolean);
            CHECK_EQ(bool2->get(), "false");
            CHECK_EQ(bool2->get<bool>(), false);
        }

        SUBCASE("Nesting errors")
        {
            nanojsoncpp::json_size_t      error_index = 0u;
            nanojsoncpp::json_parse_error error       = nanojsoncpp::json_parse_error::no_error;

            std::string json_input_obj_255 =
                R"({"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_end":"really?"}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}})";

            auto root1 = nanojsoncpp::parse(json_input_obj_255, descriptors);
            REQUIRE(root1);
            CHECK_EQ(compute_nesting_level(root1.value(), 1), 255);

            std::string json_input_obj_256 =
                R"({"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_next":{"test_nesting":234,"test_nesting_end":"really?"}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}})";

            auto root2 = nanojsoncpp::parse(json_input_obj_256,
                                            descriptors,
                                            [&](nanojsoncpp::json_size_t _error_index, nanojsoncpp::json_parse_error _error)
                                            {
                                                error_index = _error_index;
                                                error       = _error;
                                            });
            CHECK_FALSE(root2);
            CHECK_EQ(error_index, 10200u);
            CHECK_EQ(error, nanojsoncpp::json_parse_error::max_nesting_level);

            std::string json_input_arr_255 =
                R"([234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,"really?"]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]])";

            auto root3 = nanojsoncpp::parse(json_input_arr_255, descriptors);
            REQUIRE(root3);
            CHECK_EQ(compute_nesting_level(root3.value(), 1), 255);

            std::string json_input_arr_256 =
                R"([234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,[234,"really?"]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]])";

            auto root4 = nanojsoncpp::parse(json_input_arr_256,
                                            descriptors,
                                            [&](nanojsoncpp::json_size_t _error_index, nanojsoncpp::json_parse_error _error)
                                            {
                                                error_index = _error_index;
                                                error       = _error;
                                            });
            CHECK_FALSE(root4);
            CHECK_EQ(error_index, 1275u);
            CHECK_EQ(error, nanojsoncpp::json_parse_error::max_nesting_level);
        }
    }

    TEST_CASE("Other parse errors")
    {
        std::array<nanojsoncpp::json_value_desc, 5u> descriptors;
        nanojsoncpp::json_size_t                     error_index = 0u;
        nanojsoncpp::json_parse_error                error       = nanojsoncpp::json_parse_error::no_error;

        std::string json_input_obj1 = R"(
            "test_null": null,
            "test_bool":true,
            "test_string": "My string!\n",
            "test_int": 1234,
            "test_decimal": -1.23456
        }
        )";

        std::array<nanojsoncpp::json_value_desc, 0u> empty_descriptors;
        auto                                         root1 = nanojsoncpp::parse(json_input_obj1,
                                        empty_descriptors,
                                        [&](nanojsoncpp::json_size_t _error_index, nanojsoncpp::json_parse_error _error)
                                        {
                                            error_index = _error_index;
                                            error       = _error;
                                        });
        CHECK_FALSE(root1);
        CHECK_EQ(error_index, 0u);
        CHECK_EQ(error, nanojsoncpp::json_parse_error::not_enough_memory);

        error_index = 0u;
        error       = nanojsoncpp::json_parse_error::no_error;
        auto root2  = nanojsoncpp::parse(json_input_obj1,
                                        descriptors,
                                        [&](nanojsoncpp::json_size_t _error_index, nanojsoncpp::json_parse_error _error)
                                        {
                                            error_index = _error_index;
                                            error       = _error;
                                        });
        CHECK_FALSE(root2);
        CHECK_EQ(error_index, 14u);
        CHECK_EQ(error, nanojsoncpp::json_parse_error::not_a_json_string);

        std::string json_input_obj2 = R"(
        {
            "test_null": null,
            "test_bool":true,
            "test_string": "My string!\n",
            "test_int": 1234,
            "test_decimal": -1.23456
        }
        )";

        error_index = 0u;
        error       = nanojsoncpp::json_parse_error::no_error;
        auto root3  = nanojsoncpp::parse(json_input_obj2,
                                        descriptors,
                                        [&](nanojsoncpp::json_size_t _error_index, nanojsoncpp::json_parse_error _error)
                                        {
                                            error_index = _error_index;
                                            error       = _error;
                                        });
        CHECK_FALSE(root3);
        CHECK_EQ(error_index, 143u);
        CHECK_EQ(error, nanojsoncpp::json_parse_error::not_enough_memory);

        auto root4 = nanojsoncpp::parse(json_input_obj2, descriptors);
        CHECK_FALSE(root4);

        constexpr size_t big_json_size = static_cast<size_t>(nanojsoncpp::MAX_JSON_STRING_SIZE) + 1u;
        char*            big_json      = new char[big_json_size];
        std::string_view big_json_str(big_json, big_json_size);

        error_index = 0u;
        error       = nanojsoncpp::json_parse_error::no_error;
        auto root5  = nanojsoncpp::parse(big_json_str,
                                        descriptors,
                                        [&](nanojsoncpp::json_size_t _error_index, nanojsoncpp::json_parse_error _error)
                                        {
                                            error_index = _error_index;
                                            error       = _error;
                                        });
        CHECK_FALSE(root5);
        CHECK_EQ(error_index, 0u);
        CHECK_EQ(error, nanojsoncpp::json_parse_error::json_too_big);
        delete[] big_json;

        std::string json_input_obj3 = R"(
            {
                "test_null": null,
                "test_bool":true
            },
            { "oupsiee": false }
            )";

        error_index = 0u;
        error       = nanojsoncpp::json_parse_error::no_error;
        auto root6  = nanojsoncpp::parse(json_input_obj3,
                                        descriptors,
                                        [&](nanojsoncpp::json_size_t _error_index, nanojsoncpp::json_parse_error _error)
                                        {
                                            error_index = _error_index;
                                            error       = _error;
                                        });
        CHECK_FALSE(root6);
        CHECK_EQ(error_index, 96u);
        CHECK_EQ(error, nanojsoncpp::json_parse_error::trailing_chars);

        std::string json_input_obj4 = R"(
            
            )";

        error_index = 0u;
        error       = nanojsoncpp::json_parse_error::no_error;
        auto root7  = nanojsoncpp::parse(json_input_obj4,
                                        descriptors,
                                        [&](nanojsoncpp::json_size_t _error_index, nanojsoncpp::json_parse_error _error)
                                        {
                                            error_index = _error_index;
                                            error       = _error;
                                        });
        CHECK_FALSE(root7);
        CHECK_EQ(error_index, 0u);
        CHECK_EQ(error, nanojsoncpp::json_parse_error::not_a_json_string);

        std::string json_input_obj5 = R"( {       )";

        error_index = 0u;
        error       = nanojsoncpp::json_parse_error::no_error;
        auto root8  = nanojsoncpp::parse(json_input_obj5,
                                        descriptors,
                                        [&](nanojsoncpp::json_size_t _error_index, nanojsoncpp::json_parse_error _error)
                                        {
                                            error_index = _error_index;
                                            error       = _error;
                                        });
        CHECK_FALSE(root8);
        CHECK_EQ(error_index, 10u);
        CHECK_EQ(error, nanojsoncpp::json_parse_error::unexpected_end_of_json_string);

        std::string json_input_obj6 = R"( {  "oupsie"     )";

        error_index = 0u;
        error       = nanojsoncpp::json_parse_error::no_error;
        auto root9  = nanojsoncpp::parse(json_input_obj6,
                                        descriptors,
                                        [&](nanojsoncpp::json_size_t _error_index, nanojsoncpp::json_parse_error _error)
                                        {
                                            error_index = _error_index;
                                            error       = _error;
                                        });
        CHECK_FALSE(root9);
        CHECK_EQ(error_index, 18u);
        CHECK_EQ(error, nanojsoncpp::json_parse_error::missing_value_separator);

        std::string json_input_obj7 = R"( {  "not good...":     )";

        error_index = 0u;
        error       = nanojsoncpp::json_parse_error::no_error;
        auto root10 = nanojsoncpp::parse(json_input_obj7,
                                         descriptors,
                                         [&](nanojsoncpp::json_size_t _error_index, nanojsoncpp::json_parse_error _error)
                                         {
                                             error_index = _error_index;
                                             error       = _error;
                                         });
        CHECK_FALSE(root10);
        CHECK_EQ(error_index, 23u);
        CHECK_EQ(error, nanojsoncpp::json_parse_error::unexpected_end_of_json_string);

        std::string json_input_obj8 = R"( {  "not better": 123    )";

        error_index = 0u;
        error       = nanojsoncpp::json_parse_error::no_error;
        auto root11 = nanojsoncpp::parse(json_input_obj8,
                                         descriptors,
                                         [&](nanojsoncpp::json_size_t _error_index, nanojsoncpp::json_parse_error _error)
                                         {
                                             error_index = _error_index;
                                             error       = _error;
                                         });
        CHECK_FALSE(root11);
        CHECK_EQ(error_index, 26u);
        CHECK_EQ(error, nanojsoncpp::json_parse_error::unexpected_end_of_json_string);
    }
}

// Compute the total nesting levels of a json object or array tree
static int compute_nesting_level(const nanojsoncpp::json_value& value, int current_level)
{
    int level = current_level;
    for (const auto& val : value)
    {
        if ((val.type() == nanojsoncpp::json_value_type::array) || (val.type() == nanojsoncpp::json_value_type::object))
        {
            level = compute_nesting_level(val, current_level + 1);
        }
    }

    return level;
}