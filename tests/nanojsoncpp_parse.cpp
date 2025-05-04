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
static void write_array(std::ostream& output_json, const nanojsoncpp::json_value& array, bool has_next_value);
/** @brief Write an object to the output json */
static void write_object(std::ostream& output_json, const nanojsoncpp::json_value& object, bool has_next_value);
/** @brief Write a value to the output json */
static void write_value(std::ostream& output_json, const nanojsoncpp::json_value& value, bool has_next_value);

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
                    // Writing output file
                    std::cout << "Writing output json file..." << std::endl;
                    write_value(output_json, root.value(), false);

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
static void write_array(std::ostream& output_json, const nanojsoncpp::json_value& array, bool has_next_value)
{
    char tmp[100u];
    auto left = nanojsoncpp::generate_array_begin(array.name(), std::span<char>(tmp, sizeof(tmp)));
    output_json << std::string_view(tmp, sizeof(tmp) - left.size());

    nanojsoncpp::json_size_t count = 0u;
    for (const auto& value : array)
    {
        bool next_value = (count != (array.size() - 1u));
        write_value(output_json, value, next_value);
        count++;
    }

    left = nanojsoncpp::generate_array_end(std::span<char>(tmp, sizeof(tmp)), has_next_value);
    output_json << std::string_view(tmp, sizeof(tmp) - left.size());
}

/** @brief Write an object to the output json */
static void write_object(std::ostream& output_json, const nanojsoncpp::json_value& object, bool has_next_value)
{
    char tmp[100u];
    auto left = nanojsoncpp::generate_object_begin(object.name(), std::span<char>(tmp, sizeof(tmp)));
    output_json << std::string_view(tmp, sizeof(tmp) - left.size());

    nanojsoncpp::json_size_t count = 0u;
    for (const auto& value : object)
    {
        bool next_value = (count != (object.size() - 1u));
        write_value(output_json, value, next_value);
        count++;
    }

    left = nanojsoncpp::generate_object_end(std::span<char>(tmp, sizeof(tmp)), has_next_value);
    output_json << std::string_view(tmp, sizeof(tmp) - left.size());
}

/** @brief Write a value to the output json */
static void write_value(std::ostream& output_json, const nanojsoncpp::json_value& value, bool has_next_value)
{
    static char tmp1[10000];
    static char tmp2[10000];

    switch (value.type())
    {
        case nanojsoncpp::json_value_type::null:
        {
            auto left = nanojsoncpp::generate_value(value.name(), tmp1, has_next_value);
            output_json << std::string_view(tmp1, sizeof(tmp1) - left.size());
        }
        break;

        case nanojsoncpp::json_value_type::boolean:
        {
            bool val  = value.get<bool>();
            auto left = nanojsoncpp::generate_value(value.name(), val, tmp1, has_next_value);
            output_json << std::string_view(tmp1, sizeof(tmp1) - left.size());
        }
        break;

        case nanojsoncpp::json_value_type::integer:
        {
            auto val  = value.get<nanojsoncpp::json_int_t>();
            auto left = nanojsoncpp::generate_value(value.name(), val, tmp1, has_next_value);
            output_json << std::string_view(tmp1, sizeof(tmp1) - left.size());
        }
        break;

        case nanojsoncpp::json_value_type::decimal:
        {
            auto val  = value.get<nanojsoncpp::json_float_t>();
            auto left = nanojsoncpp::generate_value(value.name(), val, tmp1, has_next_value);
            output_json << std::string_view(tmp1, sizeof(tmp1) - left.size());
        }
        break;

        case nanojsoncpp::json_value_type::string:
        {
            auto val = value.get_escaped(tmp1);

            auto left = nanojsoncpp::generate_value(value.name(), val, tmp2, has_next_value);
            output_json << std::string_view(tmp2, sizeof(tmp2) - left.size());
        }
        break;

        case nanojsoncpp::json_value_type::array:
            write_array(output_json, value, has_next_value);
            break;

        case nanojsoncpp::json_value_type::object:
            write_object(output_json, value, has_next_value);
            break;

        default:
            break;
    }
}
