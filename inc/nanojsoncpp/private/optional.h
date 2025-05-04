/*
 * Copyright (c) 2025 c-jimenez
 * SPDX-License-Identifier: MIT
 */

#ifndef NANOJSONCPP_OPTIONAL_H
#define NANOJSONCPP_OPTIONAL_H

namespace nanojsoncpp
{

/** @brief Optional value
 *         Basic and limited implementation of std::optional 
 *         without any dynamic memory management dependencies
 */
template <typename T>
class optional
{
  public:
    /** @brief Default constructor */
    optional() : m_has_value(false), m_value() { }

    /** @brief Copy constructor */
    optional(const optional& copy) = default;
    /** @brief Copy operator */
    optional& operator=(const optional& copy) = default;

    /** @brief Move constructor */
    optional(optional&& move) : m_has_value(move.m_has_value), m_value(std::move(move.m_value)) { move.clear(); }
    /** @brief Move operator */
    optional& operator=(optional&& move)
    {
        m_has_value = move.m_has_value;
        m_value     = std::move(move.m_value);
        move.clear();
        return (*this);
    }

    /** @brief Indicate if the value is present */
    bool has_value() const { return m_has_value; }

    /** @brief Implicit conversion to bool */
    operator bool() const { return m_has_value; }

    /** @brief Get the value */
    T& value() { return m_value; }

    /** @brief Get the value */
    const T& value() const { return m_value; }

    /** @brief Get the value */
    T* operator->() { return &m_value; }

    /** @brief Get the value */
    const T* operator->() const { return &m_value; }

    /** @brief Get the value */
    T& operator*() { return m_value; }

    /** @brief Get the value */
    const T& operator*() const { return m_value; }

    /** @brief Set the value */
    void set_value(const T& value)
    {
        m_value     = value;
        m_has_value = true;
    }

    /** @brief Set the value */
    void set_value(T&& value)
    {
        m_value     = std::move(value);
        m_has_value = true;
    }

    /** @brief Unset the value */
    void clear() { m_has_value = false; }

  private:
    /** @brief Indicate if the value is present */
    bool m_has_value;
    /** @brief Underlying value */
    T m_value;
};

} // namespace nanojsoncpp

#endif // NANOJSONCPP_OPTIONAL_H
