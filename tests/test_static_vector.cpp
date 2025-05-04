/*
 * Copyright (c) 2025 c-jimenez
 * SPDX-License-Identifier: MIT
 */

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>

#include <nanojsoncpp/private/static_vector.h>

TEST_SUITE("nanojsoncpp::static_vector test suite")
{

    TEST_CASE("Basic type")
    {
        // Default constructor
        nanojsoncpp::static_vector<int, 5u>        sv1;
        const nanojsoncpp::static_vector<int, 5u>& csv1 = sv1;

        CHECK(sv1.empty());
        CHECK_EQ(sv1.size(), 0u);
        CHECK_EQ(sv1.max_size(), 5u);
        CHECK_EQ(sv1.capacity(), 5u);

        size_t count = 0;
        for ([[maybe_unused]] int v : sv1)
        {
            count++;
        }
        CHECK_EQ(count, 0u);
        count = 0;
        for (auto iter = sv1.rbegin(); iter != sv1.rend(); iter++)
        {
            count++;
        }
        CHECK_EQ(count, 0u);

        // Non empty bu not full vector
        CHECK(sv1.push_back(1234));
        CHECK(sv1.push_back(5678));
        CHECK(sv1.push_back(9012));
        CHECK(sv1.push_back(3456));

        CHECK_FALSE(sv1.empty());
        CHECK_EQ(sv1.size(), 4u);
        CHECK_EQ(sv1.max_size(), 5u);
        CHECK_EQ(sv1.capacity(), 5u);

        CHECK_EQ(sv1.data(), &sv1.front());
        CHECK_EQ(sv1.front(), 1234);
        CHECK_EQ(sv1.back(), 3456);
        CHECK_EQ(csv1.data(), &csv1.front());
        CHECK_EQ(csv1.front(), 1234);
        CHECK_EQ(csv1.back(), 3456);

        std::array<int, 4u> ref_values1 = {1234, 5678, 9012, 3456};
        for (size_t i = 0u; i < sv1.size(); i++)
        {
            CHECK_EQ(sv1[i], ref_values1[i]);
        }
        for (size_t i = 0u; i < csv1.size(); i++)
        {
            CHECK_EQ(csv1[i], ref_values1[i]);
        }
        count = 0;
        for (int v : sv1)
        {
            CHECK_EQ(v, ref_values1[count]);
            count++;
        }
        CHECK_EQ(count, sv1.size());
        count = 0;
        for (int v : csv1)
        {
            CHECK_EQ(v, ref_values1[count]);
            count++;
        }
        CHECK_EQ(count, sv1.size());
        count = 0;
        for (auto iter = csv1.cbegin(); iter != csv1.cend(); iter++)
        {
            CHECK_EQ(*iter, ref_values1[count]);
            count++;
        }
        CHECK_EQ(count, sv1.size());
        count = 0;
        for (auto iter = sv1.rbegin(); iter != sv1.rend(); iter++)
        {
            count++;
            CHECK_EQ(*iter, ref_values1[sv1.size() - count]);
        }
        CHECK_EQ(count, sv1.size());
        count = 0;
        for (auto iter = csv1.rbegin(); iter != csv1.rend(); iter++)
        {
            count++;
            CHECK_EQ(*iter, ref_values1[csv1.size() - count]);
        }
        CHECK_EQ(count, sv1.size());
        count = 0;
        for (auto iter = csv1.crbegin(); iter != csv1.crend(); iter++)
        {
            count++;
            CHECK_EQ(*iter, ref_values1[csv1.size() - count]);
        }
        CHECK_EQ(count, sv1.size());

        // Copy vector
        nanojsoncpp::static_vector<int, 5u> sv2(sv1);

        CHECK_FALSE(sv2.empty());
        CHECK_EQ(sv2.size(), sv1.size());
        CHECK_EQ(sv2.max_size(), sv1.max_size());
        CHECK_EQ(sv2.capacity(), sv1.capacity());

        CHECK_EQ(sv2.data(), &sv2.front());
        CHECK_EQ(sv2.front(), sv1.front());
        CHECK_EQ(sv2.back(), sv1.back());

        for (size_t i = 0u; i < sv2.size(); i++)
        {
            CHECK_EQ(sv2[i], ref_values1[i]);
        }

        nanojsoncpp::static_vector<int, 5u> sv3;
        sv3 = sv1;

        CHECK_FALSE(sv3.empty());
        CHECK_EQ(sv3.size(), sv1.size());
        CHECK_EQ(sv3.max_size(), sv1.max_size());
        CHECK_EQ(sv3.capacity(), sv1.capacity());

        CHECK_EQ(sv3.data(), &sv3.front());
        CHECK_EQ(sv3.front(), sv1.front());
        CHECK_EQ(sv3.back(), sv1.back());

        for (size_t i = 0u; i < sv3.size(); i++)
        {
            CHECK_EQ(sv3[i], ref_values1[i]);
        }

        // Move vector
        nanojsoncpp::static_vector<int, 5u> sv4(std::move(sv2));

        CHECK_FALSE(sv4.empty());
        CHECK_EQ(sv4.size(), sv1.size());
        CHECK_EQ(sv4.max_size(), sv1.max_size());
        CHECK_EQ(sv4.capacity(), sv1.capacity());

        CHECK_EQ(sv4.data(), &sv4.front());
        CHECK_EQ(sv4.front(), sv1.front());
        CHECK_EQ(sv4.back(), sv1.back());

        for (size_t i = 0u; i < sv4.size(); i++)
        {
            CHECK_EQ(sv4[i], ref_values1[i]);
        }

        CHECK(sv2.empty());
        CHECK_EQ(sv2.size(), 0u);

        nanojsoncpp::static_vector<int, 5u> sv5;
        sv5 = std::move(sv3);

        CHECK_FALSE(sv5.empty());
        CHECK_EQ(sv5.size(), sv1.size());
        CHECK_EQ(sv5.max_size(), sv1.max_size());
        CHECK_EQ(sv5.capacity(), sv1.capacity());

        CHECK_EQ(sv5.data(), &sv5.front());
        CHECK_EQ(sv5.front(), sv1.front());
        CHECK_EQ(sv5.back(), sv1.back());

        for (size_t i = 0u; i < sv5.size(); i++)
        {
            CHECK_EQ(sv5[i], ref_values1[i]);
        }

        CHECK(sv3.empty());
        CHECK_EQ(sv3.size(), 0u);

        // Full vector
        CHECK(sv1.push_back(7890));
        CHECK_FALSE(sv1.push_back(1234));

        CHECK_FALSE(sv1.empty());
        CHECK_EQ(sv1.size(), 5u);
        CHECK_EQ(sv1.max_size(), 5u);
        CHECK_EQ(sv1.capacity(), 5u);

        CHECK_EQ(sv1.front(), 1234);
        CHECK_EQ(sv1.back(), 7890);

        std::array<int, 5u> ref_values2 = {1234, 5678, 9012, 3456, 7890};
        count                           = 0;
        for (int v : sv1)
        {
            CHECK_EQ(v, ref_values2[count]);
            count++;
        }
        CHECK_EQ(count, sv1.size());
        count = 0;
        for (auto iter = sv1.rbegin(); iter != sv1.rend(); iter++)
        {
            count++;
            CHECK_EQ(*iter, ref_values2[sv1.size() - count]);
        }
        CHECK_EQ(count, sv1.size());

        // Clear vector
        sv1.clear();

        CHECK(sv1.empty());
        CHECK_EQ(sv1.size(), 0u);
        CHECK_EQ(sv1.max_size(), 5u);
        CHECK_EQ(sv1.capacity(), 5u);

        count = 0;
        for ([[maybe_unused]] int v : sv1)
        {
            count++;
        }
        CHECK_EQ(count, 0u);
        count = 0;
        for (auto iter = sv1.rbegin(); iter != sv1.rend(); iter++)
        {
            count++;
        }
        CHECK_EQ(count, 0u);
    }

    TEST_CASE("Structured type")
    {
        // Default constructor
        nanojsoncpp::static_vector<std::string, 5u>        sv1;
        const nanojsoncpp::static_vector<std::string, 5u>& csv1 = sv1;

        CHECK(sv1.empty());
        CHECK_EQ(sv1.size(), 0u);
        CHECK_EQ(sv1.max_size(), 5u);
        CHECK_EQ(sv1.capacity(), 5u);

        size_t count = 0;
        for ([[maybe_unused]] const std::string& v : sv1)
        {
            count++;
        }
        CHECK_EQ(count, 0u);
        count = 0;
        for (auto iter = sv1.rbegin(); iter != sv1.rend(); iter++)
        {
            count++;
        }
        CHECK_EQ(count, 0u);

        // Non empty bu not full vector
        CHECK(sv1.push_back("1234"));
        CHECK(sv1.push_back("5678"));
        CHECK(sv1.push_back("9012"));
        CHECK(sv1.push_back("3456"));

        CHECK_FALSE(sv1.empty());
        CHECK_EQ(sv1.size(), 4u);
        CHECK_EQ(sv1.max_size(), 5u);
        CHECK_EQ(sv1.capacity(), 5u);

        CHECK_EQ(sv1.data(), &sv1.front());
        CHECK_EQ(sv1.front(), "1234");
        CHECK_EQ(sv1.back(), "3456");
        CHECK_EQ(csv1.data(), &csv1.front());
        CHECK_EQ(csv1.front(), "1234");
        CHECK_EQ(csv1.back(), "3456");

        std::array<std::string, 4u> ref_values1 = {"1234", "5678", "9012", "3456"};
        for (size_t i = 0u; i < sv1.size(); i++)
        {
            CHECK_EQ(sv1[i], ref_values1[i]);
        }
        for (size_t i = 0u; i < csv1.size(); i++)
        {
            CHECK_EQ(csv1[i], ref_values1[i]);
        }
        count = 0;
        for (const std::string& v : sv1)
        {
            CHECK_EQ(v, ref_values1[count]);
            count++;
        }
        CHECK_EQ(count, sv1.size());
        count = 0;
        for (const std::string& v : csv1)
        {
            CHECK_EQ(v, ref_values1[count]);
            count++;
        }
        CHECK_EQ(count, sv1.size());
        count = 0;
        for (auto iter = csv1.cbegin(); iter != csv1.cend(); iter++)
        {
            CHECK_EQ(*iter, ref_values1[count]);
            count++;
        }
        CHECK_EQ(count, sv1.size());
        count = 0;
        for (auto iter = sv1.rbegin(); iter != sv1.rend(); iter++)
        {
            count++;
            CHECK_EQ(*iter, ref_values1[sv1.size() - count]);
        }
        CHECK_EQ(count, sv1.size());
        count = 0;
        for (auto iter = csv1.rbegin(); iter != csv1.rend(); iter++)
        {
            count++;
            CHECK_EQ(*iter, ref_values1[csv1.size() - count]);
        }
        CHECK_EQ(count, sv1.size());
        count = 0;
        for (auto iter = csv1.crbegin(); iter != csv1.crend(); iter++)
        {
            count++;
            CHECK_EQ(*iter, ref_values1[csv1.size() - count]);
        }
        CHECK_EQ(count, sv1.size());

        // Copy vector
        nanojsoncpp::static_vector<std::string, 5u> sv2(sv1);

        CHECK_FALSE(sv2.empty());
        CHECK_EQ(sv2.size(), sv1.size());
        CHECK_EQ(sv2.max_size(), sv1.max_size());
        CHECK_EQ(sv2.capacity(), sv1.capacity());

        CHECK_EQ(sv2.data(), &sv2.front());
        CHECK_EQ(sv2.front(), sv1.front());
        CHECK_EQ(sv2.back(), sv1.back());

        for (size_t i = 0u; i < sv2.size(); i++)
        {
            CHECK_EQ(sv2[i], ref_values1[i]);
        }

        nanojsoncpp::static_vector<std::string, 5u> sv3;
        sv3 = sv1;

        CHECK_FALSE(sv3.empty());
        CHECK_EQ(sv3.size(), sv1.size());
        CHECK_EQ(sv3.max_size(), sv1.max_size());
        CHECK_EQ(sv3.capacity(), sv1.capacity());

        CHECK_EQ(sv3.data(), &sv3.front());
        CHECK_EQ(sv3.front(), sv1.front());
        CHECK_EQ(sv3.back(), sv1.back());

        for (size_t i = 0u; i < sv3.size(); i++)
        {
            CHECK_EQ(sv3[i], ref_values1[i]);
        }

        // Move vector
        nanojsoncpp::static_vector<std::string, 5u> sv4(std::move(sv2));

        CHECK_FALSE(sv4.empty());
        CHECK_EQ(sv4.size(), sv1.size());
        CHECK_EQ(sv4.max_size(), sv1.max_size());
        CHECK_EQ(sv4.capacity(), sv1.capacity());

        CHECK_EQ(sv4.data(), &sv4.front());
        CHECK_EQ(sv4.front(), sv1.front());
        CHECK_EQ(sv4.back(), sv1.back());

        for (size_t i = 0u; i < sv4.size(); i++)
        {
            CHECK_EQ(sv4[i], ref_values1[i]);
        }

        CHECK(sv2.empty());
        CHECK_EQ(sv2.size(), 0u);

        nanojsoncpp::static_vector<std::string, 5u> sv5;
        sv5 = std::move(sv3);

        CHECK_FALSE(sv5.empty());
        CHECK_EQ(sv5.size(), sv1.size());
        CHECK_EQ(sv5.max_size(), sv1.max_size());
        CHECK_EQ(sv5.capacity(), sv1.capacity());

        CHECK_EQ(sv5.data(), &sv5.front());
        CHECK_EQ(sv5.front(), sv1.front());
        CHECK_EQ(sv5.back(), sv1.back());

        for (size_t i = 0u; i < sv5.size(); i++)
        {
            CHECK_EQ(sv5[i], ref_values1[i]);
        }

        CHECK(sv3.empty());
        CHECK_EQ(sv3.size(), 0u);

        // Full vector
        const std::string val1 = "7890";
        const std::string val2 = "1234";
        CHECK(sv1.push_back(val1));
        CHECK_FALSE(sv1.push_back(val2));
        CHECK_FALSE(sv1.push_back("1234"));

        CHECK_FALSE(sv1.empty());
        CHECK_EQ(sv1.size(), 5u);
        CHECK_EQ(sv1.max_size(), 5u);
        CHECK_EQ(sv1.capacity(), 5u);

        CHECK_EQ(sv1.front(), "1234");
        CHECK_EQ(sv1.back(), "7890");

        std::array<std::string, 5u> ref_values2 = {"1234", "5678", "9012", "3456", "7890"};
        count                                   = 0;
        for (const std::string& v : sv1)
        {
            CHECK_EQ(v, ref_values2[count]);
            count++;
        }
        CHECK_EQ(count, sv1.size());
        count = 0;
        for (auto iter = sv1.rbegin(); iter != sv1.rend(); iter++)
        {
            count++;
            CHECK_EQ(*iter, ref_values2[sv1.size() - count]);
        }
        CHECK_EQ(count, sv1.size());

        // Clear vector
        sv1.clear();

        CHECK(sv1.empty());
        CHECK_EQ(sv1.size(), 0u);
        CHECK_EQ(sv1.max_size(), 5u);
        CHECK_EQ(sv1.capacity(), 5u);

        count = 0;
        for ([[maybe_unused]] const std::string& v : sv1)
        {
            count++;
        }
        CHECK_EQ(count, 0u);
        count = 0;
        for (auto iter = sv1.rbegin(); iter != sv1.rend(); iter++)
        {
            count++;
        }
        CHECK_EQ(count, 0u);
    }
}
