#ifndef _missio_json_utils_hpp
#define _missio_json_utils_hpp

#pragma once

#include "types.hpp"

#include <system_error>
#include <charconv>
#include <iterator>
#include <limits>


namespace  missio::json
{

template <typename Number>
string to_string(Number number)
{
    char str[std::numeric_limits<Number>::digits10 + 3];

    auto [ptr, err] = std::to_chars(std::begin(str), std::end(str), number);

    if(err != std::errc{})
    {
        throw std::system_error{std::make_error_code(err)};
    }

    return {std::begin(str), ptr};
}

template <typename T, std::ptrdiff_t N>
constexpr auto ssize(T const (&)[N]) noexcept
{
    return N;
}

} // namespace missio::json

#endif // _missio_json_utils_hpp
