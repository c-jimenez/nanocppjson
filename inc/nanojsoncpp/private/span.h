/*
 * Copyright (c) 2025 c-jimenez
 * SPDX-License-Identifier: MIT
 */

#ifndef NANOJSONCPP_STD_SPAN_H
#define NANOJSONCPP_STD_SPAN_H

// Backport of some of the std::span features from C++20 to C++17

#include <array>
#include <cstddef>
#include <iterator>
#include <limits>
#include <memory>
#include <stdexcept>
#include <type_traits>

namespace std
{

// Macro to throw exceptions only when it is enabled in the compiler options
// Works only for GCC and clang compilers
#if __cpp_exceptions
#define THROW_EXCEPTION(exception_type, message) throw exception_type(message)
#else
#define THROW_EXCEPTION(exception_type, message) abort()
#endif

// Forward declaration of std::span from C++20
template <typename T, std::size_t Extent>
class span;

// Dynamic extent definition from C++20
inline constexpr std::size_t dynamic_extent = std::numeric_limits<std::size_t>::max();

// std::type_identity from C++20
template <class T>
struct type_identity
{
    using type = T;
};
template <class T>
using type_identity_t = typename type_identity<T>::type;

// std::iter_reference_t from C++20
template <class T>
using iter_reference_t = decltype(*std::declval<T&>());

// Implementation details
namespace details
{

template <typename>
struct is_span : std::false_type
{
};

template <typename T, std::size_t E>
struct is_span<span<T, E>> : std::true_type
{
};

template <typename>
struct is_std_array : std::false_type
{
};

template <typename T, std::size_t N>
struct is_std_array<std::array<T, N>> : std::true_type
{
};

template <typename, typename = void>
struct has_size_and_data : std::false_type
{
};

template <typename T>
struct has_size_and_data<T, void_t<decltype(std::size(std::declval<T>())), decltype(std::data(std::declval<T>()))>> : std::true_type
{
};

template <typename C, typename U = std::remove_cv_t<std::remove_reference_t<C>>>
struct is_container
{
    static constexpr bool value = !is_span<U>::value && !is_std_array<U>::value && !std::is_array<U>::value && has_size_and_data<C>::value;
};

template <typename C, typename U = std::remove_cv_t<std::remove_reference_t<C>>>
static constexpr bool is_container_v = is_container<C, U>::value;

template <typename, typename, typename = void>
struct is_container_element_type_compatible : std::false_type
{
};

template <typename T, typename E>
struct is_container_element_type_compatible<
    T,
    E,
    typename std::enable_if_t<!std::is_same_v<typename std::remove_cv_t<decltype(std::data(std::declval<T>()))>, void> &&
                              std::is_convertible_v<std::remove_pointer_t<decltype(std::data(std::declval<T>()))> (*)[], E (*)[]>>>
    : std::true_type
{
};

template <typename T, typename E>
static constexpr bool is_container_element_type_compatible_v = is_container_element_type_compatible<T, E>::value;

} // namespace details

// std::span from C++20
template <typename T, std::size_t Extent = std::dynamic_extent>
class span
{
  public:
    // Nested types
    using element_type     = T;
    using value_type       = std::remove_cv_t<std::remove_reference_t<T>>;
    using size_type        = std::size_t;
    using difference_type  = std::ptrdiff_t;
    using pointer          = T*;
    using const_pointer    = const T*;
    using reference        = T&;
    using const_reference  = const T&;
    using iterator         = T*;
    using reverse_iterator = std::reverse_iterator<iterator>;

    // Constructors
    constexpr span() noexcept : _storage(nullptr, 0u) { static_assert((Extent == std::dynamic_extent) || (Extent == 0u)); }

    template <
        class It,
        typename = std::enable_if_t<std::is_convertible_v<std::remove_reference_t<std::iter_reference_t<It>> (*)[], element_type (*)[]>>>
    constexpr span(It first, size_type count) : _storage(first, count)
    {
    }

    template <
        class It,
        class End,
        typename = std::enable_if_t<std::is_convertible_v<std::remove_reference_t<std::iter_reference_t<It>> (*)[], element_type (*)[]>>>
    constexpr span(It first, End last) : _storage(first, static_cast<size_type>(last - first))
    {
        if ((Extent != std::dynamic_extent) && (size() != Extent))
        {
            THROW_EXCEPTION(std::out_of_range, "Invalid Extend value for span::span(It first, End last)");
        }
    }

    template <std::size_t N, typename = std::enable_if_t<(Extent == std::dynamic_extent) || (N == Extent)>>
    constexpr span(std::type_identity_t<element_type> (&arr)[N]) noexcept : _storage(arr, N)
    {
    }

    template <class U, std::size_t N, typename = std::enable_if_t<(Extent == std::dynamic_extent) || (N == Extent)>>
    constexpr span(std::array<U, N>& arr) noexcept : _storage(arr.data(), N)
    {
    }

    template <class U, std::size_t N, typename = std::enable_if_t<(Extent == std::dynamic_extent) || (N == Extent)>>
    constexpr span(const std::array<U, N>& arr) noexcept : _storage(arr.data(), N)
    {
    }

    template <typename Container,
              std::size_t N = Extent,
              typename      = std::enable_if_t<((Extent == std::dynamic_extent) || (N == Extent)) && details::is_container_v<Container> &&
                                               details::is_container_element_type_compatible_v<Container&, element_type>>>
    constexpr span(Container& cont) : _storage(std::data(cont), std::size(cont))
    {
        if ((Extent != std::dynamic_extent) && (size() != Extent))
        {
            THROW_EXCEPTION(std::out_of_range, "Invalid Extend value for span::span(Container& cont)");
        }
    }

    template <typename Container,
              std::size_t N = Extent,
              typename      = std::enable_if_t<((Extent == std::dynamic_extent) || (N == Extent)) && details::is_container_v<Container> &&
                                               details::is_container_element_type_compatible_v<const Container&, element_type>>>
    constexpr span(const Container& cont) : _storage(std::data(cont), std::size(cont))
    {
        if ((Extent != std::dynamic_extent) && (size() != Extent))
        {
            THROW_EXCEPTION(std::out_of_range, "Invalid Extend value for span::span(Container& cont)");
        }
    }

    template <class U,
              std::size_t N,
              typename = std::enable_if_t<((Extent == std::dynamic_extent) || (N == Extent)) &&
                                          std::is_convertible_v<U (*)[], element_type (*)[]>>>
    constexpr span(const std::span<U, N>& source) noexcept : _storage(source.data(), source.size())
    {
    }

    constexpr span(const span& other) noexcept = default;

    constexpr span& operator=(const span& other) noexcept = default;

    // Iterators
    constexpr iterator         begin() const noexcept { return data(); }
    constexpr iterator         end() const noexcept { return &data()[size()]; }
    constexpr reverse_iterator rbegin() const noexcept { return reverse_iterator(end()); }
    constexpr reverse_iterator rend() const noexcept { return reverse_iterator(begin()); }

    // Element access
    constexpr reference front() const { return *begin(); }
    constexpr reference back() const { return *rbegin(); }
    constexpr reference at(size_type pos) const
    {
        if (pos >= size())
        {
            THROW_EXCEPTION(std::out_of_range, "Invalid [pos] value for span::at()");
        }
        return data()[pos];
    }
    constexpr reference operator[](size_type idx) const { return data()[idx]; }
    constexpr pointer   data() const noexcept { return _storage._data; }

    // Observers
    constexpr size_type size() const noexcept { return _storage._size; }
    constexpr size_type size_bytes() const noexcept { return size() * sizeof(element_type); }
    constexpr bool      empty() const noexcept { return size() == 0; }

    // Subviews
    template <std::size_t Count>
    constexpr std::span<element_type, Count> first() const
    {
        if constexpr (Extent != std::dynamic_extent)
        {
            static_assert(Count <= Extent);
        }
        else
        {
            if (Count > size())
            {
                THROW_EXCEPTION(std::out_of_range, "Invalid [Count] value for span::first()");
            }
        }
        return std::span<element_type, Count>(begin(), Count);
    }
    constexpr std::span<element_type, std::dynamic_extent> first(size_type count) const
    {
        if (count > size())
        {
            THROW_EXCEPTION(std::out_of_range, "Invalid [count] value for span::first()");
        }
        return std::span<element_type, std::dynamic_extent>(begin(), count);
    }
    template <std::size_t Count>
    constexpr std::span<element_type, Count> last() const
    {
        if constexpr (Extent != std::dynamic_extent)
        {
            static_assert(Count <= Extent);
        }
        else
        {
            if (Count > size())
            {
                THROW_EXCEPTION(std::out_of_range, "Invalid [Count] value for span::last()");
            }
        }
        return std::span<element_type, Count>(data() + (size() - Count), Count);
    }
    constexpr std::span<element_type, std::dynamic_extent> last(size_type count) const
    {
        if (count > size())
        {
            THROW_EXCEPTION(std::out_of_range, "Invalid [count] value for span::last()");
        }
        return std::span<element_type, std::dynamic_extent>(data() + (size() - count), count);
    }
    template <std::size_t Offset, std::size_t Count = std::dynamic_extent>
    constexpr std::span<element_type, Count> subspan() const
    {
        if constexpr (Count == std::dynamic_extent)
        {
            if constexpr (Extent != std::dynamic_extent)
            {
                static_assert(Offset < Extent);
            }
            else
            {
                if (Offset >= size())
                {
                    THROW_EXCEPTION(std::out_of_range, "Invalid [Offset] value for span::subspan()");
                }
            }
            return std::span<element_type, Count>(data() + Offset, size() - Offset);
        }
        else if constexpr (Extent != std::dynamic_extent)
        {
            static_assert(Count <= Extent);
            static_assert(Offset < Extent);
            static_assert(Count + Offset <= Extent);
        }
        else
        {
            if (Count > size())
            {
                THROW_EXCEPTION(std::out_of_range, "Invalid [Count] value for span::subspan()");
            }
            if ((size() != 0) && (Offset >= size()))
            {
                THROW_EXCEPTION(std::out_of_range, "Invalid [Offset] value for span::subspan()");
            }
            if ((Count + Offset) > size())
            {
                THROW_EXCEPTION(std::out_of_range, "Invalid [Count+Offset] value for span::subspan()");
            }
        }
        return std::span<element_type, Count>(data() + Offset, Count);
    }
    constexpr std::span<element_type, std::dynamic_extent> subspan(size_type offset, size_type count = std::dynamic_extent) const
    {
        if ((size() != 0) && (offset >= size()))
        {
            THROW_EXCEPTION(std::out_of_range, "Invalid [offset] value for span::subspan()");
        }
        if (count == std::dynamic_extent)
        {
            return std::span<element_type, std::dynamic_extent>(data() + offset, size() - offset);
        }
        else
        {
            if (count > size())
            {
                THROW_EXCEPTION(std::out_of_range, "Invalid [count] value for span::subspan()");
            }
            if ((count + offset) > size())
            {
                THROW_EXCEPTION(std::out_of_range, "Invalid [count+offset] value for span::subspan()");
            }
            return std::span<element_type, std::dynamic_extent>(data() + offset, count);
        }
    }

  private:
    // Structure to store internal data
    template <typename U, std::size_t E>
    struct storage
    {
        constexpr storage(U* _d, size_t) : _data(_d) { }

        U*                         _data;
        static constexpr size_type _size = E;
    };

    template <typename U>
    struct storage<U, std::dynamic_extent>
    {
        constexpr storage(U* _d, size_t _s) : _data(_d), _size(_s) { }

        U*        _data;
        size_type _size;
    };

    // Data members
    static constexpr std::size_t _extent = Extent;
    storage<T, Extent>           _storage;
};

// std::as_bytes and std::as_writable_bytes from C++20
template <class T, std::size_t N>
std::span<const std::byte, (N == std::dynamic_extent) ? std::dynamic_extent : sizeof(T) * N> as_bytes(std::span<T, N> s) noexcept
{
    return {reinterpret_cast<const std::byte*>(s.data()), s.size_bytes()};
}
template <class T, std::size_t N>
std::span<std::byte, (N == std::dynamic_extent) ? std::dynamic_extent : sizeof(T) * N> as_writable_bytes(std::span<T, N> s) noexcept
{
    return {reinterpret_cast<std::byte*>(s.data()), s.size_bytes()};
}

} // namespace std

#endif // NANOJSONCPP_STD_SPAN_H
