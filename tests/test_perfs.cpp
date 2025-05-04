/*
 * Copyright (c) 2025 c-jimenez
 * SPDX-License-Identifier: MIT
 */

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>

#include <nanojsoncpp/nanojsoncpp.h>

#include <nlohmann/json.hpp>

#include <tiny-json.h>

#include <chrono>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

using namespace std::string_literals;
using namespace std::chrono_literals;

TEST_SUITE("Performance tests")
{
    TEST_CASE("Large json file")
    {
        std::filesystem::path test_file_path = TEST_FILES_DIR;
        test_file_path.append("test_large.json"s);

        std::ifstream input_json(test_file_path, std::fstream::in | std::fstream::binary | std::fstream::ate);
        REQUIRE(input_json.is_open());

        std::string input_json_str;
        auto        filesize = input_json.tellg();
        input_json.seekg(0, input_json.beg);
        input_json_str.resize(static_cast<size_t>(filesize));
        input_json.read(&input_json_str[0], filesize);
        input_json.close();

        SUBCASE("nanojsoncpp - parse only")
        {
            nanojsoncpp::json_size_t                  error_index = 0u;
            nanojsoncpp::json_parse_error             error       = nanojsoncpp::json_parse_error::no_error;
            std::vector<nanojsoncpp::json_value_desc> descriptors(1000000);

            auto callback = [&](nanojsoncpp::json_size_t _error_index, nanojsoncpp::json_parse_error _error)
            {
                error_index = _error_index;
                error       = _error;
            };
            const auto start = std::chrono::high_resolution_clock::now();
            for (int i = 0; i < 1000; i++)
            {
                auto root = nanojsoncpp::parse(input_json_str, descriptors, callback);
                if (!root)
                {
                    CHECK(false);
                }
            }
            const auto stop = std::chrono::high_resolution_clock::now();

            const auto test_duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
            std::cout << "[nanojsoncpp] : " << test_duration.count() << "µs" << std::endl;
        }

        SUBCASE("nlohmann::json - parse only")
        {
            const auto start = std::chrono::high_resolution_clock::now();
            for (int i = 0; i < 1000; i++)
            {
                auto root = nlohmann::json::parse(input_json_str);
                if (root.is_null())
                {
                    CHECK(false);
                }
            }
            const auto stop = std::chrono::high_resolution_clock::now();

            const auto test_duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
            std::cout << "[nlohmann::json] : " << test_duration.count() << "µs" << std::endl;
        }

        SUBCASE("tiny-json - parse only")
        {
            std::vector<json_t> pool(1000000);

            std::vector<std::string> input_json_strs;
            for (int i = 0; i < 1000; i++)
            {
                input_json_strs.push_back(input_json_str);
            }

            const auto start = std::chrono::high_resolution_clock::now();
            for (int i = 0; i < 1000; i++)
            {
                json_t const* parent =
                    json_create(const_cast<char*>(input_json_strs[i].c_str()), pool.data(), static_cast<unsigned int>(pool.size()));
                if (parent == nullptr)
                {
                    CHECK(false);
                }
            }
            const auto stop = std::chrono::high_resolution_clock::now();

            const auto test_duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
            std::cout << "[tiny-json] : " << test_duration.count() << "µs" << std::endl;
        }
    }
}
