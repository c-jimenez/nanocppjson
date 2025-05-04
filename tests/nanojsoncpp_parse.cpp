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

/** @brief Value indentation */
static const std::string VALUE_INDENT = "  "s;

/** @brief Write an array to the output json */
static void write_array(std::ostream& output_json, const nanojsoncpp::json_value& array, const std::string& indent);
/** @brief Write an object to the output json */
static void write_object(std::ostream& output_json, const nanojsoncpp::json_value& object, const std::string& indent);
/** @brief Write a value to the output json */
static void write_value(std::ostream& output_json, const nanojsoncpp::json_value& value, const std::string& indent);

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
                    if (root->type() == nanojsoncpp::json_value_type::array)
                    {
                        write_array(output_json, root.value(), "");
                    }
                    else
                    {
                        write_object(output_json, root.value(), "");
                    }
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
static void write_array(std::ostream& output_json, const nanojsoncpp::json_value& array, const std::string& indent)
{
    output_json << "[ ";

    std::string              value_indent = indent + VALUE_INDENT;
    nanojsoncpp::json_size_t count        = 0u;
    for (const auto& value : array)
    {
        write_value(output_json, value, value_indent);
        count++;
        if (count != array.size())
        {
            output_json << ", ";
        }
    }
    output_json << " ]";
}

/** @brief Write an object to the output json */
static void write_object(std::ostream& output_json, const nanojsoncpp::json_value& object, const std::string& indent)
{
    output_json << "{" << std::endl;

    std::string              value_indent = indent + VALUE_INDENT;
    nanojsoncpp::json_size_t count        = 0u;
    for (const auto& value : object)
    {
        output_json << value_indent << "\"" << value.name() << "\": ";
        write_value(output_json, value, value_indent);
        count++;
        if (count != object.size())
        {
            output_json << "," << std::endl;
        }
    }
    output_json << std::endl;

    output_json << indent << "}";
}

/** @brief Write a value to the output json */
static void write_value(std::ostream& output_json, const nanojsoncpp::json_value& value, const std::string& indent)
{
    switch (value.type())
    {
        case nanojsoncpp::json_value_type::null:
        case nanojsoncpp::json_value_type::boolean:
        case nanojsoncpp::json_value_type::integer:
        case nanojsoncpp::json_value_type::decimal:
            output_json << value.get();
            break;

        case nanojsoncpp::json_value_type::string:
            output_json << "\"" << value.get() << "\"";
            break;

        case nanojsoncpp::json_value_type::array:
            write_array(output_json, value, indent);
            break;

        case nanojsoncpp::json_value_type::object:
            write_object(output_json, value, indent);
            break;

        default:
            break;
    }
};