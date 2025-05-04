/*
 * Copyright (c) 2025 c-jimenez
 * SPDX-License-Identifier: MIT
 */

#ifndef NANOJSONCPP_STATIC_VECTOR_H
#define NANOJSONCPP_STATIC_VECTOR_H

#include <array>
#include <cstddef>

namespace nanojsoncpp
{

/** @brief Vector of values
 *         Basic and limited implementation of std::vector 
 *         without any dynamic memory management dependencies
 */
template <typename T, size_t MAX_SIZE>
class static_vector
{
  public:
    // Member types
    using value_type             = T;
    using pointer                = value_type*;
    using const_pointer          = const value_type*;
    using reference              = value_type&;
    using const_reference        = const value_type&;
    using iterator               = value_type*;
    using const_iterator         = const value_type*;
    using size_type              = size_t;
    using reverse_iterator       = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    /** @brief Default constructor */
    static_vector() : m_count(0u), m_values() { }

    /** @brief Copy constructor */
    explicit static_vector(const static_vector& copy) = default;
    /** @brief Copy operator */
    static_vector& operator=(const static_vector& copy) = default;

    /** @brief Move constructor */
    explicit static_vector(static_vector&& move) : m_count(move.m_count), m_values(std::move(move.m_values)) { move.m_count = 0u; }
    /** @brief Move operator */
    static_vector& operator=(static_vector&& move)
    {
        m_count      = move.m_count;
        m_values     = std::move(move.m_values);
        move.m_count = 0u;
        return (*this);
    }

    // Element access

    /** @brief Access specified element */
    reference operator[](size_t pos) { return m_values[pos]; }

    /** @brief Access specified element */
    const_reference operator[](size_t pos) const { return m_values[pos]; }

    /** @brief Access the first element */
    reference front() { return m_values[0u]; }

    /** @brief Access the first element */
    const_reference front() const { return m_values[0u]; }

    /** @brief Access the last element */
    reference back() { return m_values[m_count - 1u]; }

    /** @brief Access the last element */
    const_reference back() const { return m_values[m_count - 1u]; }

    /** @brief Direct access to the underlying contiguous storage */
    pointer data() { return m_values.data(); }

    /** @brief Direct access to the underlying contiguous storage */
    const_pointer data() const { return m_values.data(); }

    // Iterators

    /** @brief Returns an iterator to the beginning */
    iterator begin() { return m_values.begin(); }

    /** @brief Returns an iterator to the beginning */
    const_iterator begin() const { return m_values.begin(); }

    /** @brief Returns an iterator to the beginning */
    const_iterator cbegin() const { return m_values.cbegin(); }

    /** @brief Returns an iterator to the end */
    iterator end() { return iterator(&m_values[m_count]); }

    /** @brief Returns an iterator to the end */
    const_iterator end() const { return iterator(&m_values[m_count]); }

    /** @brief Returns an iterator to the end */
    const_iterator cend() const { return const_iterator(&m_values[m_count]); }

    /** @brief Returns an iterator to the beginning */
    reverse_iterator rbegin() { return reverse_iterator(end()); }

    /** @brief Returns an iterator to the beginning */
    const_reverse_iterator rbegin() const { return const_reverse_iterator(end()); }

    /** @brief Returns an iterator to the beginning */
    const_reverse_iterator crbegin() const { return const_reverse_iterator(end()); }

    /** @brief Returns an iterator to the end */
    reverse_iterator rend() { return reverse_iterator(begin()); }

    /** @brief Returns an iterator to the end */
    const_reverse_iterator rend() const { return const_reverse_iterator(begin()); }

    /** @brief Returns an iterator to the end */
    const_reverse_iterator crend() const { return const_reverse_iterator(begin()); }

    // Capacity

    /** @brief Checks whether the container is empty */
    bool empty() const { return (m_count == 0u); }

    /** @brief Returns the number of elements */
    size_type size() const { return m_count; }

    /** @brief Returns the maximum possible number of elements */
    size_type max_size() const { return MAX_SIZE; }

    /** @brief Returns the number of elements that can be held in currently allocated storage */
    size_type capacity() const { return MAX_SIZE; }

    // Modifiers

    /** Clears the contents */
    void clear() { m_count = 0u; }

    /** @brief Adds an element to the end */
    bool push_back(const T& value)
    {
        bool ret = false;

        if (m_count != MAX_SIZE)
        {
            m_values[m_count] = value;
            m_count++;
            ret = true;
        }

        return ret;
    }

    /** @brief Adds an element to the end */
    bool push_back(T&& value)
    {
        bool ret = false;

        if (m_count != MAX_SIZE)
        {
            m_values[m_count] = std::move(value);
            m_count++;
            ret = true;
        }

        return ret;
    }

  private:
    /** @brief Number of values in the vector */
    size_type m_count;
    /** @brief Underlying array */
    std::array<T, MAX_SIZE> m_values;
};

} // namespace nanojsoncpp

#endif // NANOJSONCPP_STATIC_VECTOR_H
