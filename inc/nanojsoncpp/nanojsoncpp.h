/*
 * Copyright (c) 2025 c-jimenez
 * SPDX-License-Identifier: MIT
 */

#ifndef NANOJSONCPP_H
#define NANOJSONCPP_H

// Standard headers
#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <limits>
#include <optional>
#include <string_view>
#include <type_traits>

// C++20 headers
#if __cplusplus > 201703L
#include <span>
#else
#include <nanojsoncpp/private/span.h>
#endif // __cplusplus > 201703L

// nanojsoncpp headers
#include <nanojsoncpp/private/config.h>
#include <nanojsoncpp/private/utils.h>

namespace nanojsoncpp
{

/** @brief Maximum size of a json string */
constexpr json_size_t MAX_JSON_STRING_SIZE = std::numeric_limits<json_size_t>::max();

/** @brief Maximum nesting level of objects and arrays in a json string */
constexpr uint8_t MAX_NESTING_LEVEL = std::numeric_limits<uint8_t>::max();

/** @brief Json value type */
enum class json_value_type : uint8_t
{
    /** @brief Null value */
    null,
    /** @brief Boolean value */
    boolean,
    /** @brief String value */
    string,
    /** @brief Integer value */
    integer,
    /** @brief Integer or floating point value */
    decimal,
    /** @brief Array */
    array,
    /** @brief Object */
    object
};

/** @brief Json value descriptor */
struct json_value_desc
{
    /** @brief Nesting level of the json value */
    uint8_t nesting_level;
    /** @brief Type of the json value */
    json_value_type type;
    /** @brief Index of the begining of the name of the value in the json string */
    json_size_t name_start;
    /** @brief Size of the name of the value in bytes in the json string */
    json_size_t name_size;
    /** @brief Index of the begining of the value in the json string */
    json_size_t value_start;
    /** @brief Size of the value in bytes in the json string */
    json_size_t value_size;
};

/** @brief Json value */
class json_value
{
  public:
    /** @brief Default constructor, needed for array and std::optional => Do not use!! */
    json_value() : m_json_string(), m_desc(nullptr) { }
    /** @brief Constructor */
    json_value(const std::string_view& json_string, json_value_desc& desc) : m_json_string(json_string), m_desc(&desc) { }

    /** @brief Get the type of the value */
    json_value_type type() const { return m_desc->type; }

    /** @brief Indicate if the value is of the null type */
    bool is_null() const { return (type() == json_value_type::null); }

    /** @brief Indicate if the value is of the boolean type */
    bool is_bool() const { return (type() == json_value_type::boolean); }

    /** @brief Indicate if the value is of the string type */
    bool is_string() const { return (type() == json_value_type::string); }

    /** @brief Indicate if the value is of the integer type */
    bool is_integer() const { return (type() == json_value_type::integer); }

    /** @brief Indicate if the value is of the decimal type */
    bool is_decimal() const { return ((type() == json_value_type::decimal) || is_integer()); }

    /** @brief Indicate if the value is of the array type */
    bool is_array() const { return (type() == json_value_type::array); }

    /** @brief Indicate if the value is of the object type */
    bool is_object() const { return (type() == json_value_type::object); }

    /** @brief Get the name of the value */
    std::string_view name() const { return m_json_string.substr(m_desc->name_start, m_desc->name_size); }

    /** @brief Get the size of the value :
     *         - For objects : number of members
     *         - For arrays : number of elements
     *         - For other types : number of bytes of the string representation of the value
     */
    json_size_t size() const { return m_desc->value_size; }

    /** @brief Get the value as an integer or a boolean */
    template <typename T, std::enable_if_t<std::is_integral_v<T>, bool> = true>
    T get() const
    {
        if constexpr (std::is_same_v<T, bool>)
        {
            return (m_json_string.substr(m_desc->value_start, m_desc->value_size) == std::string_view("true"));
        }
        else
        {
            return static_cast<T>(to_int(m_json_string.substr(m_desc->value_start, m_desc->value_size)));
        }
    }

    /** @brief Get the value as a floating point */
    template <typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
    T get() const
    {
        return static_cast<T>(to_float(m_json_string.substr(m_desc->value_start, m_desc->value_size)));
    }

    /** @brief Get the value as a string view */
    std::string_view get() const { return m_json_string.substr(m_desc->value_start, m_desc->value_size); }

    /** @brief Get the value as an escaped string view doing an in-situ replacement.
     *         Warning: Using this function, the input JSON string
     *                  will be used as output buffer for the escaped
     *                  string and thus will be modified!!
     */
    std::string_view get_escaped() const
    {
        std::string_view unescaped_string = get();
        return get_escaped(std::span<char>(const_cast<char*>(unescaped_string.data()), unescaped_string.size()));
    }

    /** @brief Get the value as an escaped string view */
    std::string_view get_escaped(const std::span<char>& buffer) const
    {
        std::string_view escaped_string;
        std::string_view unescaped_string = get();

        if (buffer.size() >= unescaped_string.size())
        {
            bool        escaped      = false;
            json_size_t buffer_index = 0u;
            for (char c : unescaped_string)
            {
                if (escaped)
                {
                    // Escaped char
                    switch (c)
                    {
                        case '\\':
                        case '/':
                        case '"':
                            // Char can be copied as-is
                            break;

                        case 'n':
                            c = '\n';
                            break;

                        case 'r':
                            c = '\r';
                            break;

                        case 'b':
                            c = '\b';
                            break;

                        case 'f':
                            c = '\f';
                            break;

                        case 't':
                            c = '\t';
                            break;

                        default:
                            // Unknown char
                            break;
                    }

                    // Add escaped char to buffer
                    buffer[buffer_index] = c;
                    buffer_index++;
                    escaped = false;
                }
                else
                {
                    if (c == '\\')
                    {
                        // Next is an escaped char
                        escaped = true;
                    }
                    else
                    {
                        // Regular char => copy to buffer
                        buffer[buffer_index] = c;
                        buffer_index++;
                    }
                }
            }

            // Build resuting string view on top of provided buffer
            escaped_string = std::string_view(buffer.data(), buffer_index);
        }

        return escaped_string;
    }

    /** @brief Get a member value (only for object types) */
    std::optional<json_value> get(const std::string_view& name) const
    {
        std::optional<json_value> value;

        if (type() == json_value_type::object)
        {
            for (const auto& val : (*this))
            {
                if (val.name() == name)
                {
                    value = val;
                    break;
                }
            }
        }

        return value;
    }

    /** @brief Get an array value (only for array types) */
    std::optional<json_value> get(json_size_t index) const
    {
        std::optional<json_value> value;

        if (type() == json_value_type::array)
        {
            json_size_t val_index = 0u;
            for (const auto& val : (*this))
            {
                if (val_index == index)
                {
                    value = val;
                    break;
                }
                else
                {
                    val_index++;
                }
            }
        }

        return value;
    }

    /** @brief Get a member value (only for object types) */
    std::optional<json_value> operator[](const std::string_view& name) const { return get(name); }

    /** @brief Get an array value (only for array types) */
    std::optional<json_value> operator[](json_size_t index) const { return get(index); }

    /** @brief Iterator class to go through objects and arrays */
    class iterator
    {
      public:
        /** @brief Constructor */
        iterator(const std::string_view& json_string, json_value_desc* desc) : m_json_string(json_string), m_desc(desc) { }

        /** @brief Increment operator */
        iterator& operator++()
        {
            // Skip nested objects/arrays
            const uint8_t nesting_level = m_desc->nesting_level;
            do
            {
                m_desc++;
            } while (m_desc->nesting_level > nesting_level);
            return (*this);
        }

        /** @brief Dereferencement operator */
        json_value operator*() const { return json_value(m_json_string, *m_desc); }

        /** @brief Inequality operator */
        bool operator!=(const iterator& other) const { return (m_desc != other.m_desc); }

      private:
        /** @brief Parsed json string */
        std::string_view m_json_string;
        /** @brief Corresponding descriptor */
        json_value_desc* m_desc;
    };

    /** @brief Get an iterator to the beginning of the value (works only with object and array values) */
    iterator begin() const { return iterator(m_json_string, (m_desc + 1u)); }

    /** @brief Get an iterator to the end of the value (works only with object and array values) */
    iterator end() const
    {
        // Default end = begin
        json_value_desc* end = (m_desc + 1u);

        // Works only for objects and arrays
        if ((m_desc->type == json_value_type::object) || (m_desc->type == json_value_type::array))
        {
            // Look for end of values => nesting level = current nesting level
            while (end->nesting_level > m_desc->nesting_level)
            {
                end++;
            }
        }
        return iterator(m_json_string, end);
    }

  private:
    /** @brief Parsed json string */
    std::string_view m_json_string;
    /** @brief Corresponding descriptor */
    json_value_desc* m_desc;
};

/** @brief Json parse error */
enum class json_parse_error
{
    /** @brief No error */
    no_error,
    /** @brief Not enough memory */
    not_enough_memory,
    /** @brief Json string too big */
    json_too_big,
    /** @brief Not a json string */
    not_a_json_string,
    /** @brief Unexpected end of json string */
    unexpected_end_of_json_string,
    /** @brief Unexpected character */
    unexpected_char,
    /** @brief Expected an object's member name */
    expected_member_name,
    /** @brief Missing separator between member name and value */
    missing_value_separator,
    /** @brief Invalid value */
    invalid_value,
    /** @brief Invalid escaped char in the string */
    invalid_escaped_char,
    /** @brief Missing end of string */
    missing_end_of_string,
    /** @brief Maximum number of nesting levels reached */
    max_nesting_level,
    /** @brief Trailing char at the end of the json string */
    trailing_chars,

    /** @brief Number of errors */
    count
};

/** @brief Error strings */
constexpr std::array<std::string_view, static_cast<size_t>(json_parse_error::count)> json_parse_error_str = {
    "no_error",
    "not_enough_memory",
    "json_too_big",
    "not_a_json_string",
    "unexpected_end_of_json_string",
    "unexpected_char",
    "expected_member_name",
    "missing_value_separator",
    "invalid_value",
    "invalid_escaped_char",
    "missing_end_of_string",
    "max_nesting_level",
    "trailing_chars"};

/** @brief Parse error handler */
using parse_error_handler_t = std::function<void(json_size_t index, json_parse_error error)>;

/** @brief Parse a json string */
std::optional<json_value> parse(const std::string_view&           json_string,
                                const std::span<json_value_desc>& descriptors,
                                parse_error_handler_t             error_handler = nullptr);

/** @brief Generate the start of an object in the specified buffer */
std::span<char> generate_object_begin(const std::string_view& name, const std::span<char>& buffer);

/** @brief Generate the end of an object in the specified buffer */
std::span<char> generate_object_end(const std::span<char>& buffer);

/** @brief Generate the start of an array in the specified buffer */
std::span<char> generate_array_begin(const std::string_view& name, const std::span<char>& buffer);

/** @brief Generate the end of an array in the specified buffer */
std::span<char> generate_array_end(const std::span<char>& buffer);

/** @brief Generate a null json value in the specified buffer */
std::span<char> generate_value(const std::string_view& name, const std::span<char>& buffer);

/** @brief Generate a boolean json value in the specified buffer */
std::span<char> generate_value(const std::string_view& name, bool value, const std::span<char>& buffer);

/** @brief Generate a string json value in the specified buffer */
std::span<char> generate_value(const std::string_view& name, const std::string_view& value, const std::span<char>& buffer);

/** @brief Generate an integer json value in the specified buffer */
std::span<char> generate_value(const std::string_view& name, json_int_t value, const std::span<char>& buffer);

/** @brief Generate a floating point json value in the specified buffer */
std::span<char> generate_value(const std::string_view& name, json_float_t value, const std::span<char>& buffer);

/** @brief Finalize json string generation */
size_t generate_finalize(const std::span<char>& inital_buffer, const std::span<char>& current_buffer);

/** @brief Get the version string of the library */
const std::string_view& get_version();

} // namespace nanojsoncpp

#endif // NANOJSONCPP_H
