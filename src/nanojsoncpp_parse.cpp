/*
 * Copyright (c) 2025 c-jimenez
 * SPDX-License-Identifier: MIT
 */

#include <nanojsoncpp/nanojsoncpp.h>

using namespace std::string_view_literals;

namespace nanojsoncpp
{

/** @brief Internal state of the json parser */
enum class parser_state : uint8_t
{
    start_of_member_name,
    member_name,
    value_separator,
    start_of_value,
    member_separator,

    count
};

/** @brief Internal context of the json parser */
struct parse_context
{
    parser_state          state              = parser_state::count;
    uint8_t               nesting_level      = 0u;
    json_size_t           index              = 0u;
    json_size_t           values_index       = 0u;
    json_value_desc*      parent_value_desc  = 0u;
    json_value_desc*      current_value_desc = 0u;
    parse_error_handler_t error_handler      = nullptr;
};

/**/
/* ------------ Internal functions declaration ------------ */
/**/

/** @brief Notify an error */
static void notify_error(parse_context& ctxt, json_parse_error error);

/** @brief Indicate if a char is in a given list of chars */
static bool is_in(char c, const std::span<const char>& list);

/** @brief Move to the next char in the input string which is not a blank char */
static std::optional<char> get_next_not_blank_char(const std::string_view& str, json_size_t& index);

/** @brief Move to the next char in the input string which is not in the provided char list */
static std::optional<char> get_next_char_not_in(const std::span<const char>& not_in_chars, const std::string_view& str, json_size_t& index);

/** @brief Move to the next expected char in the input string */
static std::optional<char> get_next_char(const std::span<const char>& expected_chars, const std::string_view& str, json_size_t& index);

/** @brief Get a string value on the input string */
static std::optional<std::string_view> get_string_value(parse_context& ctxt, const std::string_view& str);

/** @brief Parse the start of an object's member name */
static bool parse_start_of_member_name(parse_context& ctxt, const std::string_view& json_string);

/** @brief Parse the name of an object's member */
static bool parse_member_name(parse_context& ctxt, const std::string_view& json_string);

/** @brief Parse a value separator of an object's member */
static bool parse_value_separator(parse_context& ctxt, const std::string_view& json_string);

/** @brief Parse the start of a json value */
static bool parse_start_of_value(parse_context& ctxt, const std::string_view& json_string);

/** @brief Parse a member separator of an object */
static bool parse_member_separator(parse_context& ctxt, const std::string_view& json_string);

/** @brief Functions to handle each parser state */
static const std::array<bool (*)(parse_context&, const std::string_view&), static_cast<size_t>(parser_state::count)> PARSER_STATE_FUNCS = {
    parse_start_of_member_name, parse_member_name, parse_value_separator, parse_start_of_value, parse_member_separator};

/** @brief Parse a litteral constant value */
static bool parse_litteral_const_value(parse_context&          ctxt,
                                       const std::string_view& json_string,
                                       const std::string_view& expected_value,
                                       json_value_type         type);

/** @brief Parse a string value */
static bool parse_string_value(parse_context& ctxt, const std::string_view& json_string);

/** @brief Parse a decimal value */
static bool parse_decimal_value(parse_context& ctxt, const std::string_view& json_string);

/** @brief Parse a compound value (object or array) */
static bool parse_compound_value(parse_context& ctxt, const std::string_view& json_string, json_value_type type, parser_state next_state);

/** @brief Finalize a compound value parsing */
static void finalize_compound_value(parse_context& ctxt);

/**/
/* ------------ API functions ------------ */
/**/

/** @brief Parse a json string */
std::optional<json_value> parse(const std::string_view&           json_string,
                                const std::span<json_value_desc>& descriptors,
                                parse_error_handler_t             error_handler)
{
    std::optional<json_value> root;

    // Check the input json size
    parse_context ctxt;
    ctxt.index         = 0u;
    ctxt.error_handler = error_handler;
    if (json_string.size() <= MAX_JSON_STRING_SIZE)
    {
        // Check that we can at least parse 1 value
        if (!descriptors.empty())
        {
            // json string is either an object or an array
            json_size_t         start_index  = 0u;
            std::optional<char> root_start_c = get_next_not_blank_char(json_string, start_index);
            if (root_start_c)
            {
                // Initialize parser context
                ctxt.nesting_level      = 1u;
                ctxt.values_index       = 1u;
                ctxt.index              = start_index + 1u;
                ctxt.parent_value_desc  = &descriptors[0u];
                ctxt.current_value_desc = &descriptors[1u];

                // Initialize root node
                bool             success         = true;
                json_value_desc& root_value_desc = descriptors[0u];
                root_value_desc.nesting_level    = 0u;
                root_value_desc.name_start       = 0u;
                root_value_desc.name_size        = 0u;
                root_value_desc.value_start      = start_index;
                root_value_desc.value_size       = 0u;
                if (root_start_c.value() == '{')
                {
                    ctxt.state           = parser_state::start_of_member_name;
                    root_value_desc.type = json_value_type::object;
                }
                else if (root_start_c.value() == '[')
                {
                    ctxt.state           = parser_state::start_of_value;
                    root_value_desc.type = json_value_type::array;
                }
                else
                {
                    // Error : Invalid json string
                    notify_error(ctxt, json_parse_error::not_a_json_string);
                    success = false;
                }

                // Loop on whole input string
                while ((ctxt.index < json_string.size()) && (ctxt.nesting_level > 0) && success)
                {
                    auto parser_func = PARSER_STATE_FUNCS[static_cast<size_t>(ctxt.state)];
                    success          = parser_func(ctxt, json_string);

                    if (ctxt.values_index == descriptors.size())
                    {
                        // Error : No more memory available
                        notify_error(ctxt, json_parse_error::not_enough_memory);
                        success = false;
                    }
                }

                // Check trailing chars
                if (success && (ctxt.index < json_string.size()))
                {
                    json_size_t         next_index = 0u;
                    std::optional<char> c          = get_next_not_blank_char(json_string.substr(ctxt.index), next_index);
                    if (c)
                    {
                        // Error : Trailing chars
                        notify_error(ctxt, json_parse_error::trailing_chars);
                        success = false;
                    }
                }

                if (success)
                {
                    // Return json value corresponding to the root
                    root = json_value(json_string, root_value_desc);

                    // Set last descriptor to known values
                    ctxt.current_value_desc->nesting_level = 0u;
                    ctxt.current_value_desc->name_start    = 0u;
                    ctxt.current_value_desc->name_size     = 0u;
                    ctxt.current_value_desc->value_start   = 0u;
                    ctxt.current_value_desc->value_size    = 0u;
                }
            }
            else
            {
                // Error : Invalid json string
                notify_error(ctxt, json_parse_error::not_a_json_string);
            }
        }
        else
        {
            // Error : No memory available
            notify_error(ctxt, json_parse_error::not_enough_memory);
        }
    }
    else
    {
        // Error : Json strinng too big
        notify_error(ctxt, json_parse_error::json_too_big);
    }

    return root;
}

/**/
/* ------------ Internal functions ------------ */
/**/

/** @brief Notify an error */
static void notify_error(parse_context& ctxt, json_parse_error error)
{
    if (ctxt.error_handler)
    {
        ctxt.error_handler(ctxt.index, error);
    }
}

/** @brief Indicate if a char is in a given list of chars */
static bool is_in(char c, const std::span<const char>& list)
{
    bool ret = false;

    for (char ch : list)
    {
        if (ch == c)
        {
            ret = true;
            break;
        }
    }

    return ret;
}

/** @brief Move to the next char in the input string which is not a blank char */
static std::optional<char> get_next_not_blank_char(const std::string_view& str, json_size_t& index)
{
    std::optional<char>                    next_c;
    static const std::array<char, 4u>      BLANK_CHARS = {' ', '\t', '\r', '\n'};
    static const std::span<const char, 4u> BLANK_CHARS_SPAN(BLANK_CHARS);

    // Loop on whole input string
    index = 0u;
    for (char c : str)
    {
        if (!is_in(c, BLANK_CHARS_SPAN))
        {
            // Not a blank char
            next_c = c;
            break;
        }
        else
        {
            // Blank char can be ignored
            index++;
        }
    }

    return next_c;
}

/** @brief Move to the next char in the input string which is not in the provided char list */
static std::optional<char> get_next_char_not_in(const std::span<const char>& not_in_chars, const std::string_view& str, json_size_t& index)
{
    std::optional<char> next_c;

    // Loop on whole input string
    index = 0u;
    for (char c : str)
    {
        if (!is_in(c, not_in_chars))
        {
            // Does not match one of the provided chars
            next_c = c;
            break;
        }
        else
        {
            // Char can be ignored
            index++;
        }
    }

    return next_c;
}

/** @brief Move to the next expected char in the input string */
static std::optional<char> get_next_char(const std::span<const char>& expected_chars, const std::string_view& str, json_size_t& index)
{
    std::optional<char> next_c;

    // Loop on whole input string
    index = 0u;
    for (char c : str)
    {
        if (is_in(c, expected_chars))
        {
            // Match one of the expecting chars
            next_c = c;
            break;
        }
        else
        {
            // Char can be ignored
            index++;
        }
    }

    return next_c;
}

/** @brief Get a string value on the input string */
static std::optional<std::string_view> get_string_value(parse_context& ctxt, const std::string_view& str)
{
    std::optional<std::string_view> str_value;

    static const std::array<char, 2u> STR_NEXT_CHAR = {'\\', '"'};
    bool                              end_of_str    = false;
    std::string_view                  sv            = str;
    json_size_t                       str_index     = 0u;
    json_parse_error                  error         = json_parse_error::no_error;
    while (!end_of_str)
    {
        json_size_t         c_index = 0;
        std::optional<char> c       = get_next_char(STR_NEXT_CHAR, sv, c_index);
        if (c)
        {
            str_index += c_index;
            if (*c == '"')
            {
                // End of string
                str_value = str.substr(0u, str_index);
                str_index++;
                end_of_str = true;
            }
            else
            {
                // Escaped char
                if (c_index != (sv.size() - 1u))
                {
                    // Check validity
                    char escaped = sv[c_index + 1u];
                    switch (escaped)
                    {
                        case '\\':
                        case '/':
                        case '"':
                        case 'n':
                        case 'r':
                        case 'b':
                        case 'f':
                        case 't':
                            // Valid escaped char
                            str_index += 2u;
                            sv = sv.substr(c_index + 2u);
                            break;

                        default:
                            // Invalid escaped char
                            error      = json_parse_error::invalid_escaped_char;
                            end_of_str = true;
                            break;
                    }
                }
                else
                {
                    // Error: missing end of string
                    error      = json_parse_error::missing_end_of_string;
                    end_of_str = true;
                }
            }
        }
        else
        {
            // Error: missing end of string
            error      = json_parse_error::missing_end_of_string;
            end_of_str = true;
        }
    }

    ctxt.index += str_index;
    if (!str_value)
    {
        notify_error(ctxt, error);
    }

    return str_value;
}

/** @brief Parse the start of an object's member name */
static bool parse_start_of_member_name(parse_context& ctxt, const std::string_view& json_string)
{
    bool success = false;

    // Look for either a new member name or the end of the object
    json_size_t         next_index = 0u;
    std::optional<char> next       = get_next_not_blank_char(json_string.substr(ctxt.index), next_index);
    ctxt.index += static_cast<json_size_t>(next_index + 1u);
    if (next)
    {
        if (next.value() == '}')
        {
            // End of object, check if the object is empty
            if (ctxt.parent_value_desc->value_size == 0u)
            {
                finalize_compound_value(ctxt);
                success = true;
            }
            else
            {
                // Error : Expected member name
                notify_error(ctxt, json_parse_error::expected_member_name);
            }
        }
        else if (next.value() == '"')
        {
            // Next state
            ctxt.state = parser_state::member_name;
            success    = true;
        }
        else
        {
            // Error : Unexpected char
            notify_error(ctxt, json_parse_error::unexpected_char);
        }
    }
    else
    {
        // Error : Unexpected end of json string
        notify_error(ctxt, json_parse_error::unexpected_end_of_json_string);
    }

    return success;
}

/** @brief Parse the name of an object's member */
static bool parse_member_name(parse_context& ctxt, const std::string_view& json_string)
{
    bool success = false;

    // Extract member name
    json_size_t                     start_index = ctxt.index;
    std::optional<std::string_view> name        = get_string_value(ctxt, json_string.substr(ctxt.index));
    if (name)
    {
        // Save position
        ctxt.current_value_desc->name_start = start_index;
        ctxt.current_value_desc->name_size  = static_cast<json_size_t>(name->size());

        // Next state
        ctxt.state = parser_state::value_separator;
        success    = true;
    }

    return success;
}

/** @brief Parse a value separator of an object's member */
static bool parse_value_separator(parse_context& ctxt, const std::string_view& json_string)
{
    bool success = false;

    // Look for the value separator
    json_size_t         next_index = 0u;
    std::optional<char> next       = get_next_not_blank_char(json_string.substr(ctxt.index), next_index);
    ctxt.index += static_cast<json_size_t>(next_index + 1u);
    if (next && (next.value() == ':'))
    {
        // Next state
        ctxt.state = parser_state::start_of_value;
        success    = true;
    }
    else
    {
        // Error : Missing value separator
        notify_error(ctxt, json_parse_error::missing_value_separator);
    }
    return success;
}

/** @brief Parse the start of a json value */
static bool parse_start_of_value(parse_context& ctxt, const std::string_view& json_string)
{
    bool success = false;

    // Clear value name for array values
    if (ctxt.parent_value_desc->type == json_value_type::array)
    {
        ctxt.current_value_desc->name_start = 0u;
        ctxt.current_value_desc->name_size  = 0u;
    }

    // Set nesting level
    ctxt.current_value_desc->nesting_level = ctxt.nesting_level;

    // Look for the value start
    json_size_t         next_index = 0u;
    std::optional<char> next       = get_next_not_blank_char(json_string.substr(ctxt.index), next_index);
    if (next)
    {
        // Next state
        ctxt.index += next_index;
        switch (next.value())
        {
            case 'n':
            {
                // Null value
                success = parse_litteral_const_value(ctxt, json_string, "null"sv, json_value_type::null);
            }
            break;

            case 't':
            {
                // Boolean true value
                success = parse_litteral_const_value(ctxt, json_string, "true"sv, json_value_type::boolean);
            }
            break;

            case 'f':
            {
                // Boolean false value
                success = parse_litteral_const_value(ctxt, json_string, "false"sv, json_value_type::boolean);
            }
            break;

            case '\"':
            {
                // String value
                success = parse_string_value(ctxt, json_string);
            }
            break;

            case '[':
            {
                // Array value
                success = parse_compound_value(ctxt, json_string, json_value_type::array, parser_state::start_of_value);
            }
            break;

            case '{':
            {
                // Object value
                success = parse_compound_value(ctxt, json_string, json_value_type::object, parser_state::start_of_member_name);
            }
            break;

            case ']':
            {
                // End of array, only allowed on empty array
                if ((ctxt.parent_value_desc->type == json_value_type::array) && (ctxt.parent_value_desc->value_size == 0u))
                {
                    ctxt.index += static_cast<json_size_t>(next_index + 1u);
                    finalize_compound_value(ctxt);
                    success = true;
                }
                else
                {
                    // Error : Unexpected char
                    ctxt.index += next_index;
                    notify_error(ctxt, json_parse_error::unexpected_char);
                }
            }
            break;

            default:
            {
                char c = next.value();
                if ((c == '-') || ((c >= '0') && (c <= '9')))
                {
                    // Decimal value
                    success = parse_decimal_value(ctxt, json_string);
                }
                else
                {
                    // Error : invalid value
                    notify_error(ctxt, json_parse_error::invalid_value);
                }
            }
            break;
        }
    }
    else
    {
        // Error : unexpected end of json string
        ctxt.index += next_index;
        notify_error(ctxt, json_parse_error::unexpected_end_of_json_string);
    }

    return success;
}

/** @brief Parse a member separator of an object */
static bool parse_member_separator(parse_context& ctxt, const std::string_view& json_string)
{
    bool success = false;

    // Look for the member separator
    json_size_t         next_index = 0u;
    std::optional<char> next       = get_next_not_blank_char(json_string.substr(ctxt.index), next_index);
    ctxt.index += static_cast<json_size_t>(next_index + 1u);
    if (next)
    {
        success = true;
        if (next.value() == ',')
        {
            // Next state
            if (ctxt.parent_value_desc->type == json_value_type::array)
            {
                ctxt.state = parser_state::start_of_value;
            }
            else
            {
                ctxt.state = parser_state::start_of_member_name;
            }
        }
        else if ((next.value() == '}') && (ctxt.parent_value_desc->type == json_value_type::object))
        {
            // End of object
            finalize_compound_value(ctxt);
        }
        else if ((next.value() == ']') && (ctxt.parent_value_desc->type == json_value_type::array))
        {
            // End of array
            finalize_compound_value(ctxt);
        }
        else
        {
            // Error: Unexpected char
            notify_error(ctxt, json_parse_error::unexpected_char);
            success = false;
        }
    }
    else
    {
        // Error: Unexpected end of json string
        notify_error(ctxt, json_parse_error::unexpected_end_of_json_string);
    }

    return success;
}

/** @brief Parse a litteral constant value */
static bool parse_litteral_const_value(parse_context&          ctxt,
                                       const std::string_view& json_string,
                                       const std::string_view& expected_value,
                                       json_value_type         type)
{
    bool success = false;

    if (ctxt.index + expected_value.size() < json_string.size())
    {
        std::string_view val = json_string.substr(ctxt.index, expected_value.size());
        if (val == expected_value)
        {
            // Save value
            ctxt.current_value_desc->type        = type;
            ctxt.current_value_desc->value_start = ctxt.index;
            ctxt.current_value_desc->value_size  = static_cast<json_size_t>(expected_value.size());
            ctxt.current_value_desc++;
            ctxt.values_index++;

            // Update parent
            ctxt.parent_value_desc->value_size++;

            // Next state
            ctxt.index += static_cast<json_size_t>(expected_value.size());
            ctxt.state = parser_state::member_separator;

            success = true;
        }
    }
    if (!success)
    {
        // Error : invalid value
        notify_error(ctxt, json_parse_error::invalid_value);
    }

    return success;
}

/** @brief Parse a string value */
static bool parse_string_value(parse_context& ctxt, const std::string_view& json_string)
{
    bool success = false;

    json_size_t start_of_string = ctxt.index + 1u;
    if (start_of_string < json_string.size())
    {
        ctxt.index                            = start_of_string;
        std::optional<std::string_view> value = get_string_value(ctxt, json_string.substr(start_of_string));
        if (value)
        {
            // Save position
            ctxt.current_value_desc->type        = json_value_type::string;
            ctxt.current_value_desc->value_start = start_of_string;
            ctxt.current_value_desc->value_size  = static_cast<json_size_t>(value->size());
            ctxt.current_value_desc++;
            ctxt.values_index++;

            // Update parent
            ctxt.parent_value_desc->value_size++;

            // Next state
            ctxt.state = parser_state::member_separator;

            success = true;
        }
    }
    else
    {
        // Error: missing end of string
        notify_error(ctxt, json_parse_error::missing_end_of_string);
    }

    return success;
}

/** @brief Parse a decimal value */
static bool parse_decimal_value(parse_context& ctxt, const std::string_view& json_string)
{
    bool success = false;

    static const std::array<char, 13u> DECIMAL_CHARS = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '.', '-'};

    json_size_t         next_index = 0u;
    std::optional<char> next       = get_next_char_not_in(DECIMAL_CHARS, json_string.substr(ctxt.index), next_index);
    if (next)
    {
        // Extract value
        std::string_view val = json_string.substr(ctxt.index, next_index);

        // Remove sign to check chars validity
        if (val[0] == '-')
        {
            val = val.substr(1u);
        }
        if (!val.empty())
        {
            bool is_integer = true;
            success         = true;
            for (char c : val)
            {
                if (c == '.')
                {
                    if (is_integer)
                    {
                        is_integer = false;
                    }
                    else
                    {
                        // '.' is present more than once in the value
                        success = false;
                        break;
                    }
                }
                else
                {
                    if (c == '-')
                    {
                        // Sign is present more than once in the value
                        success = false;
                        break;
                    }
                }
            }
            if (success)
            {
                // Save position
                if (is_integer)
                {
                    ctxt.current_value_desc->type = json_value_type::integer;
                }
                else
                {
                    ctxt.current_value_desc->type = json_value_type::decimal;
                }
                ctxt.current_value_desc->value_start = ctxt.index;
                ctxt.current_value_desc->value_size  = next_index;
                ctxt.current_value_desc++;
                ctxt.values_index++;

                // Update parent
                ctxt.parent_value_desc->value_size++;

                // Next state
                ctxt.index += next_index;
                ctxt.state = parser_state::member_separator;
            }
        }
    }
    if (!success)
    {
        // Error: invalid value
        notify_error(ctxt, json_parse_error::invalid_value);
    }

    return success;
}

/** @brief Parse a compound value (object or array) */
static bool parse_compound_value(parse_context& ctxt, const std::string_view& json_string, json_value_type type, parser_state next_state)
{
    bool success = false;

    (void)json_string;

    // Check nesting level
    if (ctxt.nesting_level != MAX_NESTING_LEVEL)
    {
        // Update parent
        ctxt.parent_value_desc->value_size++;
        ctxt.parent_value_desc = ctxt.current_value_desc;

        // Save position
        ctxt.index++;
        ctxt.current_value_desc->type        = type;
        ctxt.current_value_desc->value_start = ctxt.index;
        ctxt.current_value_desc->value_size  = 0u;
        ctxt.current_value_desc++;
        ctxt.values_index++;

        // Next state
        ctxt.nesting_level++;
        ctxt.state = next_state;

        success = true;
    }
    else
    {
        // Error: Maximum nesting level
        notify_error(ctxt, json_parse_error::max_nesting_level);
    }

    return success;
}

/** @brief Finalize a compound value parsing */
static void finalize_compound_value(parse_context& ctxt)
{
    // Decrease nesting level
    ctxt.nesting_level--;

    // Update parent
    while ((ctxt.parent_value_desc->nesting_level != 0) && (ctxt.parent_value_desc->nesting_level >= ctxt.nesting_level))
    {
        ctxt.parent_value_desc--;
    }

    // Next state
    ctxt.state = parser_state::member_separator;
}

} // namespace nanojsoncpp
