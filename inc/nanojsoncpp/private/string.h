/*
 * Copyright (c) 2025 c-jimenez
 * SPDX-License-Identifier: MIT
 */

#ifndef NANOJSONCPP_STRING_H
#define NANOJSONCPP_STRING_H

// Standard headers
#include <array>
#include <string_view>

// C++20 headers
#if __cplusplus > 201703L
#include <span>
#else
#include <nanojsoncpp/private/span.h>
#endif // __cplusplus > 201703L

namespace nanojsoncpp
{

/** @brief Allocate memory from a contiguous memory block without memory consumption overhead
 *         Do not provide deallocation.
 */
class simple_allocator
{
  public:
    /** 
     * @brief Constructor 
     * @param mem_block Memory block to use for allocation
     */
    simple_allocator(const std::span<std::byte>& mem_block) : m_mem_block_size(mem_block.size()), m_mem_block(mem_block) { }

    // Do not allow copy and move operations
    simple_allocator(const simple_allocator&)            = delete;
    simple_allocator(simple_allocator&&)                 = delete;
    simple_allocator& operator=(const simple_allocator&) = delete;
    simple_allocator& operator=(simple_allocator&&)      = delete;

    /** @brief Get the total size in bytes of the memory block */
    size_t size() const { return m_mem_block_size; }

    /** @brief Get the available memory size in bytes of the memory block */
    size_t available() const { return m_mem_block.size(); }

    /** 
     * @brief Allocate a block of memory for [count] elements of type [T] 
     * @param count Number of elements of type [T] to allocate
     * @return Pointer to the allocated memory if enough memory is available, nullptr otherwise
     */
    template <typename T>
    T* alloc(size_t count)
    {
        T*           mem            = nullptr;
        const size_t alloc_size     = count * sizeof(T);
        const size_t available_size = available();
        if (alloc_size <= available_size)
        {
            mem = reinterpret_cast<T*>(m_mem_block.data());
            if (alloc_size != available_size)
            {
                m_mem_block = m_mem_block.subspan(alloc_size);
            }
            else
            {
                m_mem_block = std::span<std::byte>();
            }
        }
        return mem;
    }

  private:
    /** @brief Size of the memory block */
    const size_t m_mem_block_size;
    /** Memory block to use for allocation */
    std::span<std::byte> m_mem_block;
};

/** @brief Allocate memory from a contiguous memory block statically allocated in the class without memory consumption overhead
 *         Do not provide deallocation.
 */
template <size_t SIZE_IN_BYTES>
class static_allocator : public simple_allocator
{
  public:
    /** @brief Constructor */
    static_allocator() : simple_allocator(m_mem_block) { }

  private:
    /** Memory block to use for allocation */
    std::array<std::byte, SIZE_IN_BYTES> m_mem_block;
};

/** 
 *  @brief Helper class to store a string either dynamically allocated using  
 *         provided allocator or allocated elsewhere.
 *         The string cannot be modified afterward.
 */
class string
{
  public:
    /** @brief Set the string value from an already allocated memory */
    bool set(const std::string_view& str)
    {
        bool ret = false;

        if (m_str.empty())
        {
            m_str = str;
            ret   = true;
        }

        return ret;
    }

    /** @brief Set the string value by using the provided allocator */
    bool set(const std::string_view& str, simple_allocator& allocator)
    {
        bool ret = false;

        if (m_str.empty())
        {
            char* buff = allocator.alloc<char>(str.size());
            if (buff)
            {
                memcpy(buff, str.data(), str.size());
                m_str = std::string_view(buff, str.size());
                ret   = true;
            }
        }

        return ret;
    }

    /** @brief Get the string value */
    const std::string_view& get() const { return m_str; }

    /** @brief Get the string value */
    operator const std::string_view&() const { return m_str; }

  private:
    /** @brief Underlying string */
    std::string_view m_str;
};

} // namespace nanojsoncpp

#endif // NANOJSONCPP_STRING_H
