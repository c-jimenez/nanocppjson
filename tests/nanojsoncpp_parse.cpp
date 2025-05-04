/*
 * Copyright (c) 2025 nanojsoncpp
 * SPDX-License-Identifier: MIT
 */

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <nanojsoncpp/nanojsoncpp.h>

using namespace std::string_literals;

/** @brief Write an array to the output json */
static std::span<char> write_array(const std::span<char>& output_json, const nanojsoncpp::json_value& array);
/** @brief Write an object to the output json */
static std::span<char> write_object(const std::span<char>& output_json, const nanojsoncpp::json_value& object);
/** @brief Write a value to the output json */
static std::span<char> write_value(const std::span<char>& output_json, const nanojsoncpp::json_value& value);

/** @brief Entry point */
int main(int argc, char* argv[])
{
    int ret = EXIT_FAILURE;

    // Check args
    bool        print_help = false;
    std::string input_json_path;
    std::string output_json_path;
    if (argc == 2)
    {
        input_json_path  = argv[1];
        output_json_path = input_json_path + ".nanojsoncpp";
    }
    else if (argc == 3)
    {
        input_json_path  = argv[1];
        output_json_path = argv[2];
    }
    else
    {
        print_help = true;
    }
    if (print_help)
    {
        std::cout << "Usage: nanojsoncpp_parse input_json_path [output_json_path]" << std::endl;
    }
    else
    {
        // Open input file
        std::cout << "Opening input json file..." << std::endl;

        std::ifstream input_json(input_json_path, std::fstream::in | std::fstream::binary | std::fstream::ate);
        if (input_json.is_open())
        {
            // Read whole file
            std::string input_json_str;
            auto        filesize = input_json.tellg();
            input_json.seekg(0, input_json.beg);
            input_json_str.resize(static_cast<size_t>(filesize));
            input_json.read(&input_json_str[0], filesize);
            input_json.close();

            // Parse json
            std::cout << "Parsing input json file..." << std::endl;

            nanojsoncpp::json_size_t                  error_index = 0u;
            nanojsoncpp::json_parse_error             error       = nanojsoncpp::json_parse_error::no_error;
            std::vector<nanojsoncpp::json_value_desc> descriptors(1000000);
            auto                                      root = nanojsoncpp::parse(input_json_str,
                                           descriptors,
                                           [&](nanojsoncpp::json_size_t _error_index, nanojsoncpp::json_parse_error _error)
                                           {
                                               error_index = _error_index;
                                               error       = _error;
                                           });
            if (root)
            {
                // Open output file
                std::cout << "Opening output json file..." << std::endl;

                std::ofstream output_json(output_json_path, std::fstream::out | std::fstream::binary);
                if (output_json.is_open())
                {
                    // Generating JSON string
                    static char json_string[1000000];
                    auto        left             = write_value(json_string, root.value());
                    size_t      json_string_size = nanojsoncpp::generate_finalize(json_string, left);

                    // Writing output file
                    std::cout << "Writing output json file..." << std::endl;
                    output_json << std::string_view(json_string, json_string_size);

                    // Close output file
                    output_json.close();

                    std::cout << "Done!" << std::endl;

                    ret = EXIT_SUCCESS;
                }
                else
                {
                    std::cout << "Error : Unable to open output file => " << output_json_path << std::endl;
                }
            }
            else
            {
                std::cout << "Error : Unable to parse input json => error = "
                          << nanojsoncpp::json_parse_error_str[static_cast<size_t>(error)] << ", index = " << error_index << std::endl;
            }
        }
        else
        {
            std::cout << "Error : Unable to open input file => " << input_json_path << std::endl;
        }
    }

    return ret;
}

/** @brief Write an array to the output json */
static std::span<char> write_array(const std::span<char>& output_json, const nanojsoncpp::json_value& array)
{
    auto left = nanojsoncpp::generate_array_begin(array.name(), output_json);

    for (const auto& value : array)
    {
        left = write_value(left, value);
    }

    left = nanojsoncpp::generate_array_end(left);

    return left;
}

/** @brief Write an object to the output json */
static std::span<char> write_object(const std::span<char>& output_json, const nanojsoncpp::json_value& object)
{
    auto left = nanojsoncpp::generate_object_begin(object.name(), output_json);

    for (const auto& value : object)
    {
        left = write_value(left, value);
    }

    left = nanojsoncpp::generate_object_end(left);

    return left;
}

/** @brief Write a value to the output json */
static std::span<char> write_value(const std::span<char>& output_json, const nanojsoncpp::json_value& value)
{
    std::span<char> left;

    switch (value.type())
    {
        case nanojsoncpp::json_value_type::null:
        {
            left = nanojsoncpp::generate_value(value.name(), output_json);
        }
        break;

        case nanojsoncpp::json_value_type::boolean:
        {
            bool val = value.get<bool>();
            left     = nanojsoncpp::generate_value(value.name(), val, output_json);
        }
        break;

        case nanojsoncpp::json_value_type::integer:
        {
            auto val = value.get<nanojsoncpp::json_int_t>();
            left     = nanojsoncpp::generate_value(value.name(), val, output_json);
        }
        break;

        case nanojsoncpp::json_value_type::decimal:
        {
            auto val = value.get<nanojsoncpp::json_float_t>();
            left     = nanojsoncpp::generate_value(value.name(), val, output_json);
        }
        break;

        case nanojsoncpp::json_value_type::string:
        {
            auto val = value.get_escaped();
            left     = nanojsoncpp::generate_value(value.name(), val, output_json);
        }
        break;

        case nanojsoncpp::json_value_type::array:
            left = write_array(output_json, value);
            break;

        case nanojsoncpp::json_value_type::object:
            left = write_object(output_json, value);
            break;

        default:
            break;
    }

    return left;
}
