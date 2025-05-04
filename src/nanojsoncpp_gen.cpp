/*
 * Copyright (c) 2025 c-jimenez
 * SPDX-License-Identifier: MIT
 */

#include <nanojsoncpp/nanojsoncpp.h>

#include <utility>

namespace nanojsoncpp
{

/** @brief Represenation of a null value in a json string */
static constexpr std::string_view NULL_VALUE = "null";

/** @brief Represenation of a boolean value in a json string */
static constexpr std::array<std::string_view, 2u> BOOL_VALUE = {"false", "true"};

/** @brief Chars needed to be escaped and their escaped representation */
static constexpr std::array<std::pair<char, std::string_view>, 8u> ESCAPED_CHARS = {std::pair<char, std::string_view>{'\\', R"(\\)"},
                                                                                    std::pair<char, std::string_view>{'/', R"(\/)"},
                                                                                    std::pair<char, std::string_view>{'"', R"(\")"},
                                                                                    std::pair<char, std::string_view>{'\n', R"(\n)"},
                                                                                    std::pair<char, std::string_view>{'\r', R"(\r)"},
                                                                                    std::pair<char, std::string_view>{'\b', R"(\b)"},
                                                                                    std::pair<char, std::string_view>{'\f', R"(\f)"},
                                                                                    std::pair<char, std::string_view>{'\t', R"(\t)"}};

/**/
/* ------------ Internal functions declaration ------------ */
/**/

/** @brief Get the subspan starting at a given offset
 *         Returns an empty span if the offset is bigger
 *         than the size of the provided span
 *         => Avoid exception thrown by the span class in this case
 */
static std::span<char> get_subspan(const std::span<char>& buffer, size_t offset);

/** @brief Add a unique char to the specified buffer */
static std::span<char> add_char(char c, const std::span<char>& buffer);

/** @brief Add the name of a member of an object in the specified buffer */
static std::span<char> add_member_name(const std::string_view& name, const std::span<char>& buffer);

/** @brief Add an escaped string to the specified buffer */
static std::span<char> add_escaped_string(const std::string_view& str, const std::span<char>& buffer);

/** @brief Finalize a composite value by removing the last extra ',' if exist */
static std::span<char> finalize_composite(const std::span<char>& buffer);

/**/
/* ------------ API functions ------------ */
/**/

/** @brief Generate the start of an object in the specified buffer */
std::span<char> generate_object_begin(const std::string_view& name, const std::span<char>& buffer)
{
    std::span<char> left = add_member_name(name, buffer);
    left                 = add_char('{', left);
    return left;
}

/** @brief Generate the end of an object in the specified buffer */
std::span<char> generate_object_end(const std::span<char>& buffer)
{
    std::span<char> left = finalize_composite(buffer);
    left                 = add_char('}', left);
    left                 = add_char(',', left);
    return left;
}

/** @brief Generate the start of an array in the specified buffer */
std::span<char> generate_array_begin(const std::string_view& name, const std::span<char>& buffer)
{
    std::span<char> left = add_member_name(name, buffer);
    left                 = add_char('[', left);
    return left;
}

/** @brief Generate the end of an array in the specified buffer */
std::span<char> generate_array_end(const std::span<char>& buffer)
{
    std::span<char> left = finalize_composite(buffer);
    left                 = add_char(']', left);
    left                 = add_char(',', left);
    return left;
}

/** @brief Generate a null json value in the specified buffer */
std::span<char> generate_value(const std::string_view& name, const std::span<char>& buffer)
{
    std::span<char> left;
    std::span<char> left_name = add_member_name(name, buffer);

    if (left_name.size() >= NULL_VALUE.size())
    {
        memcpy(left_name.data(), NULL_VALUE.data(), NULL_VALUE.size());
        left = get_subspan(left_name, NULL_VALUE.size());
        left = add_char(',', left);
    }

    return left;
}

/** @brief Generate a boolean json value in the specified buffer */
std::span<char> generate_value(const std::string_view& name, bool value, const std::span<char>& buffer)
{
    std::span<char> left;
    std::span<char> left_name = add_member_name(name, buffer);

    const std::string_view& str_value = BOOL_VALUE[static_cast<size_t>(value)];
    if (left_name.size() >= str_value.size())
    {
        memcpy(left_name.data(), str_value.data(), str_value.size());
        left = get_subspan(left_name, str_value.size());
        left = add_char(',', left);
    }

    return left;
}

/** @brief Generate a string json value in the specified buffer */
std::span<char> generate_value(const std::string_view& name, const std::string_view& value, const std::span<char>& buffer)
{
    std::span<char> left = add_member_name(name, buffer);

    left = add_escaped_string(value, left);
    left = add_char(',', left);

    return left;
}

/** @brief Generate an integer json value in the specified buffer */
std::span<char> generate_value(const std::string_view& name, json_int_t value, const std::span<char>& buffer)
{
    std::span<char> left;
    std::span<char> left_name = add_member_name(name, buffer);

    size_t count = to_string(value, left_name);
    if (count > 0)
    {
        left = get_subspan(left_name, count);
        left = add_char(',', left);
    }

    return left;
}

/** @brief Generate a floating point json value in the specified buffer */
std::span<char> generate_value(const std::string_view& name, json_float_t value, const std::span<char>& buffer)
{
    std::span<char> left;
    std::span<char> left_name = add_member_name(name, buffer);

    size_t count = to_string(value, left_name);
    if (count > 0)
    {
        left = get_subspan(left_name, count);
        left = add_char(',', left);
    }

    return left;
}

/** @brief Finalize json string generation */
size_t generate_finalize(const std::span<char>& inital_buffer, const std::span<char>& current_buffer)
{
    std::span<char> current   = finalize_composite(current_buffer);
    size_t          json_size = inital_buffer.size() - current.size();
    return json_size;
}

/**/
/* ------------ Internal functions ------------ */
/**/

/** @brief Get the subspan starting at a given offset
 *         Returns an empty span if the offset is bigger
 *         than the size of the provided span
 *         => Avoid exception thrown by the span class in this case
 */
static std::span<char> get_subspan(const std::span<char>& buffer, size_t offset)
{
    std::span<char> sub;

    if (offset < buffer.size())
    {
        sub = buffer.subspan(offset);
    }

    return sub;
}

/** @brief Add a unique char to the specified buffer */
static std::span<char> add_char(char c, const std::span<char>& buffer)
{
    std::span<char> left;

    if (!buffer.empty())
    {
        buffer[0u] = c;
        left       = get_subspan(buffer, 1u);
    }

    return left;
}

/** @brief Add the name of a member of an object in the specified buffer */
static std::span<char> add_member_name(const std::string_view& name, const std::span<char>& buffer)
{
    std::span<char> left = buffer;

    if (!name.empty())
    {
        left = add_escaped_string(name, buffer);
        left = add_char(':', left);
    }

    return left;
}

/** @brief Add an escaped string to the specified buffer */
static std::span<char> add_escaped_string(const std::string_view& str, const std::span<char>& buffer)
{
    std::span<char> left;

    // String start
    left = add_char('"', buffer);
    if (!left.empty())
    {
        // Escaped string
        size_t count     = 0u;
        size_t max_count = left.size();
        char*  buff      = left.data();
        for (char c : str)
        {
            bool found = false;
            for (const auto& [to_escape, escaped] : ESCAPED_CHARS)
            {
                if (c == to_escape)
                {
                    for (char e : escaped)
                    {
                        *buff = e;
                        buff++;
                        count++;
                        if (count == max_count)
                        {
                            break;
                        }
                    }
                    found = true;
                }
            }
            if (!found)
            {
                *buff = c;
                buff++;
                count++;
            }
            if (count == max_count)
            {
                break;
            }
        }
        left = get_subspan(left, count);

        // String end
        left = add_char('"', left);
    }

    return left;
}

/** @brief Finalize a composite value by removing the last extra ',' if exist */
static std::span<char> finalize_composite(const std::span<char>& buffer)
{
    std::span<char> left = buffer;
    if (!buffer.empty())
    {
        char* previous = (buffer.data() - 1);
        if (*previous == ',')
        {
            left = std::span<char>(previous, buffer.size() + 1u);
        }
    }
    return left;
}

} // namespace nanojsoncpp
