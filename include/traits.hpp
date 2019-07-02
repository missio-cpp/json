#ifndef _missio_json_traits_hpp
#define _missio_json_traits_hpp

#pragma once

#include "types.hpp"

#include <type_traits>


namespace missio::json
{

template <typename, typename = void>
struct convert;

} // namespace missio::json

namespace missio::json::traits
{

template <typename...>
struct make_void { using type = void; };

template <typename... Ts>
using void_t = typename make_void<Ts...>::type;


template <typename>
struct is_bounded_array : std::false_type {};

template <typename T, std::size_t N>
struct is_bounded_array<T[N]> : std::true_type {};

template <typename T>
inline constexpr bool is_bounded_array_v = is_bounded_array<T>::value;


template <typename, typename = void>
struct is_sequence_container : std::false_type {};

template <typename T>
struct is_sequence_container
<
    T,
    void_t
    <
        typename T::value_type,
        typename T::const_iterator,
        decltype(std::declval<T>().begin()),
        decltype(std::declval<T>().end()),
        decltype(std::declval<T>().front()),
        std::enable_if_t<!std::is_same_v<char, typename T::value_type>>,
        std::enable_if_t<std::is_constructible_v<value, typename T::value_type>>
    >
> : std::true_type {};

template <typename T>
struct is_sequence_container
<
    T,
    void_t
    <
        typename T::key_type,
        typename T::value_type,
        typename T::const_iterator,
        decltype(std::declval<T>().begin()),
        decltype(std::declval<T>().end()),
        std::enable_if_t<std::is_constructible_v<value, typename T::value_type>>,
        std::enable_if_t<std::is_same_v<typename T::key_type, typename T::value_type>>
    >
> : std::true_type {};

template <typename T>
inline constexpr bool is_sequence_container_v = is_sequence_container<T>::value;


template <typename, typename = void>
struct is_associative_container : std::false_type {};

template <typename T>
struct is_associative_container
<
    T,
    void_t
    <
        typename T::key_type,
        typename T::mapped_type,
        typename T::value_type,
        typename T::const_iterator,
        decltype(std::declval<T>().begin()),
        decltype(std::declval<T>().end()),
        std::enable_if_t<std::is_constructible_v<string, typename T::key_type>>,
        std::enable_if_t<std::is_constructible_v<value, typename T::mapped_type>>
    >
> : std::true_type {};

template <typename T>
inline constexpr bool is_associative_container_v = is_associative_container<T>::value;


template <typename, typename = void>
struct is_convertible_to_json : std::false_type {};

template <typename T>
struct is_convertible_to_json
<
    T,
    void_t
    <
        std::enable_if_t<std::is_same_v<decltype(convert<T>::to_json(std::declval<T>())), value>>
    >
> : std::true_type {};

template <typename T>
inline constexpr bool is_convertible_to_json_v = is_convertible_to_json<T>::value;


template <typename, typename = void>
struct is_convertible_from_json : std::false_type {};

template <typename T>
struct is_convertible_from_json
<
    T,
    void_t
    <
        std::enable_if_t<std::is_same_v<decltype(convert<T>::from_json(std::declval<value>())), T>>
    >
> : std::true_type {};

template <typename T>
inline constexpr bool is_convertible_from_json_v = is_convertible_from_json<T>::value;

} // namespace missio::json::traits

namespace missio::json
{

template <typename T, typename = std::enable_if_t<std::is_convertible_v<T, string>>>
using object_key = T;

template <typename T, typename = std::enable_if_t<traits::is_convertible_to_json_v<T>>>
using convertible_to_json = T;

template <typename T, typename = std::enable_if_t<traits::is_convertible_from_json_v<T>>>
using convertible_from_json = T;

} // namespace missio::json

#endif // _missio_json_traits_hpp
