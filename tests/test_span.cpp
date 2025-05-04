/*
 * Copyright (c) 2025 c-jimenez
 * SPDX-License-Identifier: MIT
 */

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>

#include <nanojsoncpp/private/span.h>

TEST_SUITE("std::span test suite")
{
    template <typename T>
    void check_iterators(const std::span<T>& s, T* data, size_t expected_count)
    {
        size_t count = 0;
        for (auto val : s)
        {
            CHECK_EQ(val, data[count]);
            count++;
        }
        CHECK_EQ(count, expected_count);

        count = 0;
        for (auto it = s.begin(); it != s.end(); it++)
        {
            CHECK_EQ(*it, data[count]);
            count++;
        }
        CHECK_EQ(count, expected_count);

        count = 0;
        for (auto it = s.rbegin(); it != s.rend(); it++)
        {
            CHECK_EQ(*it, data[(expected_count - 1u) - count]);
            count++;
        }
        CHECK_EQ(count, expected_count);
    }

    template <typename T>
    void check_access(const std::span<T>& s, T* data, size_t expected_count)
    {
        CHECK_NE(s.data(), nullptr);
        CHECK_EQ(s.front(), data[0]);
        CHECK_EQ(s.back(), data[expected_count - 1u]);
        for (size_t i = 0; i < expected_count; i++)
        {
            CHECK_EQ(s[i], data[i]);
            CHECK_EQ(s.at(i), data[i]);
        }
        CHECK_THROWS_AS(s.at(expected_count), const std::out_of_range&);
    }

    template <typename T>
    void check_subviews(const std::span<T>& s, T* data, size_t expected_count)
    {
        auto s_first_zero = s.first(0u);
        CHECK_EQ(s_first_zero.size(), 0u);

        auto s_first = s.first(expected_count / 2u);
        CHECK_EQ(s_first.size(), expected_count / 2u);
        for (size_t i = 0; i < s_first.size(); i++)
        {
            CHECK_EQ(s_first[i], data[i]);
        }

        auto s_first_stat = s.template first<3u>();
        CHECK_EQ(s_first_stat.size(), 3u);
        for (size_t i = 0; i < s_first_stat.size(); i++)
        {
            CHECK_EQ(s_first_stat[i], data[i]);
        }

        auto s_first_all = s.first(expected_count);
        check_access(s_first_all, data, expected_count);

        CHECK_THROWS_AS(s.first(expected_count + 1u), const std::out_of_range&);
        CHECK_THROWS_AS(s.template first<5u>(), const std::out_of_range&);

        auto s_last_zero = s.last(0u);
        CHECK_EQ(s_last_zero.size(), 0u);

        auto s_last = s.last(expected_count / 2u);
        CHECK_EQ(s_last.size(), expected_count / 2u);
        for (size_t i = 0; i < s_last.size(); i++)
        {
            CHECK_EQ(s_last[i], data[(expected_count - s_last.size()) + i]);
        }

        auto s_last_stat = s.template last<3u>();
        CHECK_EQ(s_last_stat.size(), 3u);
        for (size_t i = 0; i < s_last_stat.size(); i++)
        {
            CHECK_EQ(s_last_stat[i], data[(expected_count - s_last_stat.size()) + i]);
        }

        auto s_last_all = s.last(expected_count);
        check_access(s_last_all, data, expected_count);

        CHECK_THROWS_AS(s.last(expected_count + 1u), const std::out_of_range&);
        CHECK_THROWS_AS(s.template last<5u>(), const std::out_of_range&);

        auto s_subspan_count_dyn = s.subspan(1u);
        CHECK_EQ(s_subspan_count_dyn.size(), expected_count - 1u);
        for (size_t i = 0; i < s_subspan_count_dyn.size(); i++)
        {
            CHECK_EQ(s_subspan_count_dyn[i], data[i + 1u]);
        }

        auto s_subspan_count = s.subspan(1u, 2u);
        CHECK_EQ(s_subspan_count.size(), 2u);
        for (size_t i = 0; i < s_subspan_count.size(); i++)
        {
            CHECK_EQ(s_subspan_count[i], data[i + 1u]);
        }

        auto s_subspan_stat_count_dyn = s.template subspan<1u>();
        CHECK_EQ(s_subspan_stat_count_dyn.size(), expected_count - 1u);
        for (size_t i = 0; i < s_subspan_stat_count_dyn.size(); i++)
        {
            CHECK_EQ(s_subspan_stat_count_dyn[i], data[i + 1u]);
        }

        auto s_subspan_stat_count = s.template subspan<1u, 2u>();
        CHECK_EQ(s_subspan_stat_count.size(), 2u);
        for (size_t i = 0; i < s_subspan_stat_count.size(); i++)
        {
            CHECK_EQ(s_subspan_stat_count[i], data[i + 1u]);
        }

        CHECK_THROWS_AS(s.subspan(expected_count + 1u), const std::out_of_range&);
        CHECK_THROWS_AS(s.subspan(1u, expected_count + 1u), const std::out_of_range&);
        CHECK_THROWS_AS(s.template subspan<4u>(), const std::out_of_range&);
        bool did_throw = false;
        try
        {
            s.template subspan<4u, 0u>();
        }
        catch (const std::out_of_range& ex)
        {
            did_throw = true;
        }
        CHECK(did_throw);
        did_throw = false;
        try
        {
            s.template subspan<1u, 4u>();
        }
        catch (const std::out_of_range& ex)
        {
            did_throw = true;
        }
        CHECK(did_throw);
        did_throw = false;
        try
        {
            s.template subspan<0u, 5u>();
        }
        catch (const std::out_of_range& ex)
        {
            did_throw = true;
        }
        CHECK(did_throw);
    }

    template <typename T>
    void check_eq(const std::span<T>& s1, const std::span<T>& s2)
    {
        CHECK_EQ(s1.size(), s2.size());
        for (size_t i = 0; i < s1.size(); i++)
        {
            CHECK_EQ(s1[i], s2[i]);
        }
    }

    TEST_CASE("Zero sized dynamic span")
    {
        std::span<int> s;

        // Observers
        CHECK_EQ(s.size(), 0u);
        CHECK_EQ(s.size_bytes(), 0u);
        CHECK(s.empty());

        // Element access
        CHECK_EQ(s.data(), nullptr);
        CHECK_THROWS_AS(s.at(1u), const std::out_of_range&);

        // Iterators
        check_iterators(s, (int*)(nullptr), 0u);

        // Subviews
        auto s_f_0 = s.first(0u);
        CHECK(s_f_0.empty());
        CHECK_THROWS_AS(s.first(1u), const std::out_of_range&);
        auto s_l_0 = s.last(0u);
        CHECK(s_l_0.empty());
        CHECK_THROWS_AS(s.last(1u), const std::out_of_range&);
        auto s_sub_0 = s.subspan(0u, 0u);
        CHECK(s_sub_0.empty());
        CHECK_THROWS_AS(s.subspan(0u, 1u), const std::out_of_range&);
        CHECK_THROWS_AS(s.subspan(1u, 0u), const std::out_of_range&);
    }

    TEST_CASE("Dynamic span from iterator and size")
    {
        std::array<int, 4u> a = {123456, 789012, 345678, 901234};
        std::span<int>      s(a.begin(), a.size());

        // Observers
        CHECK_EQ(s.size(), 4u);
        CHECK_EQ(s.size_bytes(), 4u * sizeof(int));
        CHECK_FALSE(s.empty());

        // Element access
        check_access(s, a.data(), a.size());

        // Iterators
        check_iterators(s, a.data(), a.size());

        // Subviews
        check_subviews(s, a.data(), a.size());
    }

    TEST_CASE("Dynamic span from iterators")
    {
        std::array<int, 4u> a = {123456, 789012, 345678, 901234};
        std::span<int>      s(a.begin(), a.end());

        // Observers
        CHECK_EQ(s.size(), 4u);
        CHECK_EQ(s.size_bytes(), 4u * sizeof(int));
        CHECK_FALSE(s.empty());

        // Element access
        check_access(s, a.data(), a.size());

        // Iterators
        check_iterators(s, a.data(), a.size());

        // Subviews
        check_subviews(s, a.data(), a.size());
    }

    TEST_CASE("Dynamic span from C array")
    {
        int            a[] = {123456, 789012, 345678, 901234};
        std::span<int> s(a);

        // Observers
        CHECK_EQ(s.size(), 4u);
        CHECK_EQ(s.size_bytes(), 4u * sizeof(int));
        CHECK_FALSE(s.empty());

        // Element access
        check_access(s, a, 4u);

        // Iterators
        check_iterators(s, a, 4u);

        // Subviews
        check_subviews(s, a, 4u);
    }

    TEST_CASE("Dynamic span from array")
    {
        std::array<int, 4u> a = {123456, 789012, 345678, 901234};
        std::span<int>      s(a);

        // Observers
        CHECK_EQ(s.size(), 4u);
        CHECK_EQ(s.size_bytes(), 4u * sizeof(int));
        CHECK_FALSE(s.empty());

        // Element access
        check_access(s, a.data(), a.size());

        // Iterators
        check_iterators(s, a.data(), a.size());

        // Subviews
        check_subviews(s, a.data(), a.size());
    }

    TEST_CASE("Dynamic span from const array")
    {
        const std::array<int, 4u> a = {123456, 789012, 345678, 901234};
        std::span<const int>      s(a);

        // Observers
        CHECK_EQ(s.size(), 4u);
        CHECK_EQ(s.size_bytes(), 4u * sizeof(int));
        CHECK_FALSE(s.empty());

        // Element access
        check_access(s, a.data(), a.size());

        // Iterators
        check_iterators(s, a.data(), a.size());

        // Subviews
        check_subviews(s, a.data(), a.size());
    }

    TEST_CASE("Dynamic span from vector")
    {
        std::vector<int> v = {123456, 789012, 345678, 901234};
        std::span<int>   s(v);

        // Observers
        CHECK_EQ(s.size(), 4u);
        CHECK_EQ(s.size_bytes(), 4u * sizeof(int));
        CHECK_FALSE(s.empty());

        // Element access
        check_access(s, v.data(), v.size());

        // Iterators
        check_iterators(s, v.data(), v.size());

        // Subviews
        check_subviews(s, v.data(), v.size());
    }

    TEST_CASE("Dynamic span from const vector")
    {
        const std::vector<int> v = {123456, 789012, 345678, 901234};
        std::span<const int>   s(v);

        // Observers
        CHECK_EQ(s.size(), 4u);
        CHECK_EQ(s.size_bytes(), 4u * sizeof(int));
        CHECK_FALSE(s.empty());

        // Element access
        check_access(s, v.data(), v.size());

        // Iterators
        check_iterators(s, v.data(), v.size());

        // Subviews
        check_subviews(s, v.data(), v.size());
    }

    TEST_CASE("Dynamic span copy onstructor and operator = ")
    {
        std::array<int, 4u> a = {1, 2, 3, 4};
        std::vector<int>    v = {5, 6, 7, 8};

        std::span<int> s1 = a;
        std::span<int> s2 = v;

        std::span<int> s3 = s1.subspan(1u, 2u);
        std::span<int> s4 = s2.subspan(1u, 3u);

        auto s5(s3);
        check_eq(s3, s5);

        auto s6(s4);
        check_eq(s4, s6);

        s1 = s4;
        check_eq(s1, s4);

        s2 = s3;
        check_eq(s2, s3);

        std::span<int> s7;

        auto s8(s7);
        check_eq(s7, s8);

        s2 = s8;
        check_eq(s2, s8);
    }
}
