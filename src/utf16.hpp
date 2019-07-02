#ifndef _missio_json_utf16_hpp
#define _missio_json_utf16_hpp

#pragma once


namespace missio::utf16
{

constexpr char16_t high_surrogate_min = 0xD800;
constexpr char16_t high_surrogate_max = 0xDBFF;

constexpr char16_t low_surrogate_min = 0xDC00;
constexpr char16_t low_surrogate_max = 0xDFFF;


inline constexpr bool is_surrogate(char32_t cp) noexcept
{
    return high_surrogate_min <= cp && cp <= low_surrogate_max;
}

inline constexpr bool is_high_surrogate(char32_t cp) noexcept
{
    return high_surrogate_min <= cp && cp <= high_surrogate_max;
}

inline constexpr bool is_low_surrogate(char32_t cp) noexcept
{
    return low_surrogate_min <= cp && cp <= low_surrogate_max;
}

inline constexpr char32_t make_utf32(char16_t high, char16_t low) noexcept
{
    return 0x10000 + ((high - high_surrogate_min) << 10 | (low - low_surrogate_min));
}

} // namespace missio::utf16

#endif // _missio_json_utf16_hpp
