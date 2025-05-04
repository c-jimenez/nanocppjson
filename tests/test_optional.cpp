/*
 * Copyright (c) 2025 c-jimenez
 * SPDX-License-Identifier: MIT
 */

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>

#include <nanojsoncpp/private/optional.h>

#include <vector>

TEST_SUITE("nanojsoncpp::optional test suite")
{
    TEST_CASE("Basic type")
    {
        // Default construction
        nanojsoncpp::optional<int> opt_int;

        CHECK_FALSE(opt_int);
        CHECK_FALSE(opt_int.has_value());

        // Set a value
        opt_int.set_value(123456789);

        CHECK(opt_int);
        CHECK_EQ(opt_int.value(), 123456789);
        CHECK_EQ(*opt_int, 123456789);

        // Copy constructor
        nanojsoncpp::optional<int> opt_int2(opt_int);

        CHECK(opt_int2);
        CHECK_EQ(opt_int2.value(), 123456789);

        CHECK(opt_int);

        // Move constructor
        nanojsoncpp::optional<int> opt_int3(std::move(opt_int));

        CHECK(opt_int3);
        CHECK_EQ(opt_int3.value(), 123456789);

        CHECK_FALSE(opt_int);

        // Copy operator
        nanojsoncpp::optional<int> opt_int4;
        CHECK_FALSE(opt_int4);

        opt_int4 = opt_int2;

        CHECK(opt_int4);
        CHECK_EQ(opt_int4.value(), 123456789);

        CHECK(opt_int2);

        // Move operator
        nanojsoncpp::optional<int> opt_int5;
        CHECK_FALSE(opt_int5);

        opt_int5 = std::move(opt_int2);

        CHECK(opt_int5);
        CHECK_EQ(opt_int5.value(), 123456789);

        CHECK_FALSE(opt_int2);

        // Update value
        opt_int.set_value(0);
        *opt_int = 987654321;
        CHECK(opt_int);
        CHECK_EQ(opt_int.value(), 987654321);

        opt_int.value() = 135792468;
        CHECK(opt_int);
        CHECK_EQ(opt_int.value(), 135792468);

        // Unset the value
        opt_int.clear();

        CHECK_FALSE(opt_int);
        CHECK_FALSE(opt_int.has_value());
    }

    TEST_CASE("Structured type")
    {
        using test_type_t                = std::vector<int>;
        const test_type_t EXPECTED_VALUE = {1234, 5678, 9012, 3456, 7890};

        // Default construction
        nanojsoncpp::optional<test_type_t> opt_vect;

        CHECK_FALSE(opt_vect);
        CHECK_FALSE(opt_vect.has_value());

        // Set a value (copy)
        opt_vect.set_value(EXPECTED_VALUE);

        CHECK(opt_vect);
        CHECK_EQ(opt_vect.value(), EXPECTED_VALUE);
        CHECK_EQ(*opt_vect, EXPECTED_VALUE);
        CHECK_EQ(opt_vect->size(), EXPECTED_VALUE.size());

        // Set a value (move)
        nanojsoncpp::optional<test_type_t> opt_vect_move;
        test_type_t                        move_val = EXPECTED_VALUE;
        opt_vect_move.set_value(std::move(EXPECTED_VALUE));

        CHECK(opt_vect_move);
        CHECK_EQ(opt_vect_move.value(), EXPECTED_VALUE);

        // Update value
        *opt_vect_move = test_type_t{1, 2, 3, 4};
        CHECK(opt_vect_move);
        CHECK_EQ(opt_vect_move.value(), test_type_t{1, 2, 3, 4});

        opt_vect_move.value() = test_type_t{5, 6, 7, 8, 9};
        CHECK(opt_vect_move);
        CHECK_EQ(opt_vect_move.value(), test_type_t{5, 6, 7, 8, 9});

        // Unset the value
        opt_vect_move.clear();

        CHECK_FALSE(opt_vect_move);
        CHECK_FALSE(opt_vect_move.has_value());

        // Copy constructor
        nanojsoncpp::optional<test_type_t> opt_vect2(opt_vect);

        CHECK(opt_vect2);
        CHECK_EQ(opt_vect2.value(), EXPECTED_VALUE);

        CHECK(opt_vect);
        CHECK_EQ(opt_vect.value(), EXPECTED_VALUE);

        // Move constructor
        nanojsoncpp::optional<test_type_t> opt_vect3(std::move(opt_vect));

        CHECK(opt_vect3);
        CHECK_EQ(opt_vect3.value(), EXPECTED_VALUE);

        CHECK_FALSE(opt_vect);
        CHECK(opt_vect->empty());

        // Copy operator
        nanojsoncpp::optional<test_type_t> opt_vect4;
        CHECK_FALSE(opt_vect4);

        opt_vect4 = opt_vect2;

        CHECK(opt_vect4);
        CHECK_EQ(opt_vect4.value(), EXPECTED_VALUE);

        CHECK(opt_vect2);
        CHECK_EQ(opt_vect2.value(), EXPECTED_VALUE);

        // Move operator
        nanojsoncpp::optional<test_type_t> opt_vect5;
        CHECK_FALSE(opt_vect5);

        opt_vect5 = std::move(opt_vect2);

        CHECK(opt_vect5);
        CHECK_EQ(opt_vect5.value(), EXPECTED_VALUE);

        CHECK_FALSE(opt_vect2);
        CHECK(opt_vect2->empty());
    }
}
