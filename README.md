# nanojsoncpp

[![CodeQL Advanced](https://github.com/c-jimenez/nanojsoncpp/actions/workflows/codeql-analysis.yml/badge.svg?branch=main)](https://github.com/c-jimenez/nanojsoncpp/actions/workflows/codeql-analysis.yml)
[![Unit tests - clang](https://github.com/c-jimenez/nanojsoncpp/actions/workflows/unit-test-clang.yml/badge.svg?branch=main)](https://github.com/c-jimenez/nanojsoncpp/actions/workflows/unit-test-clang.yml)
[![Unit tests - gcc](https://github.com/c-jimenez/nanojsoncpp/actions/workflows/unit-test-gcc.yml/badge.svg?branch=main)](https://github.com/c-jimenez/nanojsoncpp/actions/workflows/unit-test-gcc.yml)

An easy to use C++ 17 json library designed for embedded systems with strong memory constraints.

## Contents


* [Overview](#overview)
  * [Major features](#major-features)
  * [Limitations](#limitations)
* [Usage](#usage)
  * [Configuration and build](#configuration-and-build)
  * [Generate JSON string](#generate-json-string)
  * [Parse JSON string](#parse-json-string)
* [The json_value object](#the-json_value-object)
  * [Using null values](#using-null-values)
  * [Using boolean values](#using-boolean-values)
  * [Using string values](#using-string-values)
  * [Using integer values](#using-integer-values)
  * [Using decimal values](#using-decimal-values)
  * [Using array values](#using-array-values)
  * [Using object values](#using-object-values)
* [Memory management](#memory-management)
* [Error handling](#error-handling)
* [Contributing](#contributing)
  * [Coding rules](#coding-rules)
  * [Issues](#issues)
  * [Workflow](#workflow)


## Overview

An easy to use C++ 17 json library designed for embedded systems with strong memory constraints.

Example fo usage:

```cpp

#include <nanojsoncpp/nanojsoncpp.h>

void f()
{
    std::string_view json_input = R"({ "my_value": 12345, "my_string": "A string!" })";

    std::array<nanojsoncpp::json_value_desc, 10u> descriptors;
    auto json_val = nanojsoncpp::parse(json_input, descriptors);
    if (json_val)
    {
        auto my_value = json_val->get("my_value");
        auto my_string = json_val->get("my_string");
        if (my_value && my_string)
        {
            int val = my_value->get<int>();
            std::string_view str = my_string->get();
            // Use val and str...
        }
    }
}

```


### Major features

* No dynamic memory allocations : in-situ parsing of provided json string
* No recursive parsing : stack consumption is totally predictible
* No use of exceptions : remove the need of code and stack usage dedicated to exception management
* No need of Run-Time Type Information (RTTI) : allow to compile without RTTI for a smaller generated code
* No external dependencies : use only standard C++ 17 features and 1 backported C++ 20 feature (std::span)
* Heavily tested with 100% of line and functions coverage and nearly 100% on branch coverage
* Low memory footprint : less than 4kBytes of ROM on armv7-m with IAR and GCC compilers when using both parsing and generating functions

### Limitations

* Maximum json input string size is 65535 bytes (can be increased to 4GBytes using configuration options)
* Maximum 255 levels of imbricated objects/arrays
* Strings: only UTF-8 strings are supported without any unicode escaping (ex: \uABCD is not supported)
* Decimals: 
  * only fractional representation is supported, not exponent representation (ex: 3.1415926 is supported, 3.141593e+00 is not supported)
  * default decimal data type is `double` (can be reduced to `float` using configuration option)
* Integers: default integer data type is `int64_t` (can be reduced to `int32_t` using configuration option)

## Usage

### Configuration and build

#### Pre-requisites

* A fully C++17 compliant compiler

That's all :)

#### Configuration

##### Using CMake

The configuration is done using the options described in [CMakeLists_Options.txt](./CMakeLists_Options.txt). From this, CMake will automatically generate the corresponding `nanojsoncpp_cfg.h` file in the build directory and make it available to the compiler.

##### Without CMake

The configuration is done by editing the [nanojsoncpp_cfg.h](./config/nanojsoncpp_cfg.h) provided in the `config` directory. Be sure to add this path in your compiler's include path. 

#### Build

##### Using CMake

For a standalone build, the build is done the usual CMake way:

```bash
mkdir build
cd build
cmake ../
cmkae --build.
```

If used in an already existing project, just add the source directory in an upper CMakeLists.txt file:

```bash
add_subdirectory(nanojsoncpp)
```

##### Without CMake

All the sources files to compile are in the `src`directory. Only the path to the `inc` directory must be added in your compiler's include path.


### Generate JSON string

Generation is done using the `nanojsoncpp::generate_XXX()` functions. All these functions have a similar behavior :

* An input buffer is provided using an `std::span<char>` object
* json string is written in the provided buffer by the `nanojsoncpp::generate_XXX()` functions
  * If the buffer is too small to write the json string, an empty `std::span<char>` object is returned
  * If the buffer is large enough, a subspan of the provided buffer pointing to the unused part of the buffer is returned so that it can be reused in further calls to `nanojsoncpp::generate_XXX()` functions
* json string is finalized and its total size is computed using the `nanojsoncpp::generate_finalize()` function

Error management is easy to implement : user application only has to check if the returned `std::span<char>` object is empty or not.

Here is an example of usage of all these functions:

```cpp
    char tmp[200];

    auto left = nanojsoncpp::generate_object_begin("", tmp);
    left      = nanojsoncpp::generate_object_begin("my_object", left);
    left      = nanojsoncpp::generate_value("value1", left);
    left      = nanojsoncpp::generate_value("value2", true, left);
    left      = nanojsoncpp::generate_value("value3", "My string!!"sv, left);
    left      = nanojsoncpp::generate_value("value4", 12345678901, left);
    left      = nanojsoncpp::generate_value("value5", 3.14, left);
    left      = nanojsoncpp::generate_object_end(left);
    left      = nanojsoncpp::generate_array_begin("my_array", left);
    left      = nanojsoncpp::generate_value("", left);
    left      = nanojsoncpp::generate_value("", false, left);
    left      = nanojsoncpp::generate_value("", "Another string!!"sv, left);
    left      = nanojsoncpp::generate_value("", 9876543210, left);
    left      = nanojsoncpp::generate_value("", 14.3, left);
    left      = nanojsoncpp::generate_array_end(left);
    left      = nanojsoncpp::generate_object_end(left);

    if (!left.empty())
    {
        size_t json_str_size = nanojsoncpp::generate_finalize(tmp, left);
        std::cout << "JSON = " << json_str_size << " : " << std::string_view(tmp, json_str_size) << std::endl;
    }
    else
    {
        std::cout << "Provided buffer too short!" << std::endl;
    }
```

Output:

```
JSON = 169 : {"my_object":{"value1":null,"value2":true,"value3":"My string!!","value4":12345678901,"value5":3.140000},"my_array":[null,false,"Another string!!",9876543210,14.300000]}
```

**Note:** The `nanojsoncpp::generate_XXX()` functions doesn't add a final ```'\0'``` char to the provide input buffer.

### Parse JSON string

Parsing is done using the `nanojsoncpp::parse()` function. The provided input json string can be of any type which can be converted to a `std::string_view` object:

* C string litterals :

```cpp
const char* json_input = R"({ "my_value": 12345, "my_string": "A string!" })";

auto json_val = nanojsoncpp::parse(json_input, descriptors);
```

* `char` array :

```cpp
char json_input[100u];
// json_input array is filled elsewhere

auto json_val = nanojsoncpp::parse({json_input, strlen(json_input)}, descriptors);
```

* `std::string` :

```cpp
std::string json_input;
// json_input string is filled elsewhere

auto json_val = nanojsoncpp::parse(json_input, descriptors);
```

The `json_val` variable is of type `std::optional<nanojsoncpp::json_value>`.

A valid json input string will return a non empty `json_val` so the success of the parse operation can simply be done by the following test :

```cpp
if (json_val)
{
    // Valid json string, data can be processed
}
else
{
    // Invalid json string
}
```

**Note**: During the parse operation, the ***input JSON string is not modified***, so a constant string can be used as input to the `nanojsoncpp::parse()`.

## The json_value object

The `nanojsoncpp::json_value` object allow to retrieve the data associated to a json value. A json value can be of one of the following types:

```cpp
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
```

The `nanojsoncpp::json_value` object provides convenient accessors to identify the data it handles :

```cpp
/** @brief Get the type of the value */
json_value_type type() const;

/** @brief Indicate if the value is of the null type */
bool is_null() const;

/** @brief Indicate if the value is of the boolean type */
bool is_bool() const;

/** @brief Indicate if the value is of the string type */
bool is_string() const;

/** @brief Indicate if the value is of the integer type */
bool is_integer() const;

/** @brief Indicate if the value is of the decimal type */
bool is_decimal() const;

/** @brief Indicate if the value is of the array type */
bool is_array() const;

/** @brief Indicate if the value is of the object type */
bool is_object() const;
```

Additionaly, it allows to retrieve the name of the data using the following method:

```cpp
/** @brief Get the name of the value */
std::string_view name() const;
```

### Using null values

A json value of ```json_value_type::null``` type has no dedicated method to interact with.

### Using boolean values

To retrieve the value of a ```json_value_type::boolean``` type, use the following method :

```cpp

if (json_val.is_boolean())
{
    bool value = json_val.get<bool>();
    // Use value
}
```

**Note:** The actual value is computed during the call to `json_val.get<bool>()`. To optimize performances when multiple accesses are needed to the json_value, store the result of the `get` operation and use it instead of calling the `get` operation multiple times.

### Using string values

A json string may contained escaped chars like `\n`, `\t`... **nanojsoncpp** allow to retrieve both the following views of the json string:

* Raw string: this is the string as it is written in the input json string
* Escaped string: a copy of the input json string where escaped chars have been remplaced by there value

To retrieve the raw value of a ```json_value_type::string```, use the following method :

```cpp

if (json_val.is_string())
{
    std::string_view value = json_val.get();
    // Use value
}
```

To retrieve the escaped value of a ```json_value_type::string```, use the following method :

```cpp

if (json_val.is_string())
{
    char tmp[100u];
    std::string_view value = json_val.get_escaped(tmp);
    // Use value
}
```

**Note 1:** The returned `std::string_view` object uses as its underlying buffer the `tmp` variable. If the `tmp` variable is not large enough to store the escaped string, the returned value will be empty. The provided buffer size must have at least the size of the raw string.

**Note 2:** The actual value is computed during the call to `json_val.get_escaped()`. To optimize performances when multiple accesses are needed to the json_value, store the result of the `get` operation and use it instead of calling the `get` operation multiple times.

An alternative method to retrieve the escaped value of a ```json_value_type::string``` without having to provide a dedicated buffer can be used depending the user application use case :

```cpp

if (json_val.is_string())
{
    std::string_view value = json_val.get_escaped();
    // Use value
}
```

**Warning**: Using this method, the input buffer used to parse the JSON string during the call to `nanojsoncpp::generate_XXX()` is used as the output buffer to escape the string. So ***the input buffer must be a non-const buffer and will be modified by this method***. 
If the input buffer points to a const area memory (ROM, MPU protected area...), the call to this method will likely provoke a bus memory fault. 

### Using integer values

To retrieve the value of a ```json_value_type::integer``` type, use the following method :

```cpp

if (json_val.is_integer())
{
    T value = json_val.get<T>();
    // Use value
}
```
Where `T`can be of any integral type which can be converted from `int64_t`using the `static_cast<>` operator (ex: `int`, `unsigned int`, `int8_t`, ...).

Example to retrieve a value as an `int16_t`:

```cpp

if (json_val.is_integer())
{
    int16_t value = json_val.get<int16_t>();
    // Use value
}
```

**Note:** The actual value is computed during the call to `json_val.get<T>()`. To optimize performances when multiple accesses are needed to the json_value, store the result of the `get` operation and use it instead of calling the `get` operation multiple times.

### Using decimal values

To retrieve the value of a ```json_value_type::decimal``` type, use the following method :

```cpp

if (json_val.is_decimal())
{
    T value = json_val.get<T>();
    // Use value
}
```
Where `T`can be of any floating point type which can be converted from `double`using the `static_cast<>` operator (ex: `float`...).

Example to retrieve a value as a `float`:

```cpp

if (json_val.is_decimal())
{
    float value = json_val.get<float>();
    // Use value
}
```

**Note:** The actual value is computed during the call to `json_val.get<T>()`. To optimize performances when multiple accesses are needed to the json_value, store the result of the `get` operation and use it instead of calling the `get` operation multiple times.

### Using array values

The following example presents the operations to access the json values contained in a ```json_value_type::array``` type:

```cpp

if (json_val.is_array())
{
    // Retrieve the number of values in the array
    json_size_t count = json_val.size();

    // Access the value stored at index 3 of the array
    auto value_at_3 = json_val[3u];
    if (value_at_3)
    {
        // The value exists, check its type
        json_value_type type = value_at_3->type();

        // Use value
    }

    // Alternative to access the value stored at index 3 of the array
    auto value_at_3_alt = json_val.get(3u);
    if (value_at_3_alt)
    {
        // The value exists, check its type
        json_value_type type = value_at_3_alt->type();
        
        // Use value
    }

    // Iterate through the whole array
    for(const auto& value: json_val)
    {
        // Check its type
        json_value_type type = value.type();

        // Use value
    }
}
```

**Note 1:** A json array may contain values of different types.

**Note 2:** The json values contained in an array have always an empty name.

### Using object values

The following example presents the operations to access the json values contained in a ```json_value_type::object``` type:

```cpp

if (json_val.is_object())
{
    // Retrieve the number of values in the object
    json_size_t count = json_val.size();

    // Access the value named 'my_int' in the object
    auto my_int_val = json_val["my_int"];
    if (my_int_val)
    {
        // Use value
        int my_int = my_int_val->get<int>();
    }

    // Alternative to access the value named 'my_int' in the object
    auto my_int_val_alt = json_val.get("my_int");
    if (my_int_val_alt)
    {
        // Use value
        int my_int = my_int_val_alt->get<int>();
    }

    // Iterate through the whole object
    for(const auto& value: json_val)
    {
        // Check its type
        json_value_type type = value.type();

        // Use value
    }
}
```

## Memory management

**nanojsoncpp** does not perform dynamic memory allocation. Instead, **nanojsoncpp** uses a pool of descriptors provided by the user application to operate.

A descriptor size in memory is 10 bytes:

```cpp
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
```

**nanojsoncpp** needs 1 descriptor per json data to parse (array, array value, object, object value).

The following json string would need 10 descriptors to be parsed:

```json
{
    "value1": 1234,
    "value2": true,
    "array1": [1.23, 2.34, 4.56],
    "object1": 
    {
        "value3": null,
        "value4": "My value"
    }
}
```

The json data structure is internally decomposed like this:

* Unamed object 
    * value1
    * value2
    * array1
        * array1[0]
        * array1[1]
        * array1[2]
    * object1
        * value3
        * value4

=> 10 descriptors

If the number of descriptors provided to the `nanojsoncpp::parse()` is insufficient, the following error code is returned : `json_error::not_enough_memory`.

## Error handling

An optional error callback can be provided to the `nanojsoncpp::parse()` function. This callback allow to locally save the error code and the index in the json string at which the error occured.

The usual usage is like this:

```cpp
std::array<nanojsoncpp::json_value_desc, 5u> descriptors;
nanojsoncpp::json_size_t                     error_index = 0u;
nanojsoncpp::json_parse_error                error       = nanojsoncpp::json_parse_error::no_error;

auto json_obj = nanojsoncpp::parse(json_input,
                                        descriptors,
                                        [&](nanojsoncpp::json_size_t _error_index, nanojsoncpp::json_parse_error _error)
                                        {
                                            error_index = _error_index;
                                            error       = _error;
                                        });
if(json_obj)
{
    // Handle json data
}
else
{
    // Error handling
    std::cout << "Parse error at index " << error_index " : code = " << static_cast<int>(error) << std::endl;
}

```

## Contributing

**nanojsoncpp** welcomes contributions. When contributing, please follow the code below.

### Coding rules

* The **.clang-format** file at the root of the source tree must not be modified (or **after** having a discussion between all the contributors)
* The code must formatted using the above mentionned file with a clang-format compliant tools (ex: Visual Studio Code)
* Every interface/class/method must be documented using the [Doxygen](https://www.doxygen.nl/) format
* No dynamic memory allocation is allowed
* Use of C/C++ macros is discouraged
* Keep code simple to understand and don't be afraid to add comments!

### Issues

Feel free to submit issues and enhancement requests.

Please help us by providing minimal reproducible examples, because source code is easier to let other people understand what happens. For crash problems on certain platforms, please bring stack dump content with the detail of the OS, compiler, etc.

Please try breakpoint debugging first, tell us what you found, see if we can start exploring based on more information been prepared.

### Workflow

Follow the "fork-and-pull" Git workflow :

* Fork the repo on GitHub
* Clone the project to your own machine
* Checkout a new branch on your fork, start developing on the branch
* Test the change before commit, Make sure the changes pass all the tests, please add test case for each new feature or bug-fix if needed.
* Commit changes to your own branch
* Push your work back up to your fork
* Submit a Pull request so that we can review your changes

**Be sure to merge the latest from "upstream" before making a pull request!**

