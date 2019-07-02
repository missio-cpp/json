#ifndef _missio_json_utf8_hpp
#define _missio_json_utf8_hpp

#pragma once

#include <algorithm>
#include <iterator>
#include <cstddef>
#include <cstdint>
#include <array>


namespace missio::utf8
{

struct code_unit_t
{
    std::uint8_t min;
    std::uint8_t max;
};

struct code_unit_sequence_t
{
    code_unit_t leading;
    code_unit_t trailing[3];
};

// http://www.unicode.org/versions/Unicode12.0.0/ch03.pdf
// Table 3-7. Well-Formed UTF-8 Byte Sequences | Page 126

constexpr code_unit_sequence_t sequences[]
{
    // single code unit sequences
    {{0x00, 0x7F}, {}},

    // double code unit sequences
    {{0xC2, 0xDF}, {{0x80, 0xBF}}},

    // triple code unit sequences
    {{0xE0, 0xE0}, {{0xA0, 0xBF}, {0x80, 0xBF}}},
    {{0xE1, 0xEC}, {{0x80, 0xBF}, {0x80, 0xBF}}},
    {{0xED, 0xED}, {{0x80, 0x9F}, {0x80, 0xBF}}},
    {{0xEE, 0xEF}, {{0x80, 0xBF}, {0x80, 0xBD}}},

    // quadruple code unit sequences
    {{0xF0, 0xF0}, {{0x90, 0xBF}, {0x80, 0xBF}, {0x80, 0xBF}}},
    {{0xF1, 0xF3}, {{0x80, 0xBF}, {0x80, 0xBF}, {0x80, 0xBF}}},
    {{0xF4, 0xF4}, {{0x80, 0x8F}, {0x80, 0xBF}, {0x80, 0xBF}}}
};

constexpr std::uint8_t first_byte_base[]{0x00, 0xC0, 0xE0, 0xF0};

// parse UTF-8 according to RFC 3629 | https://tools.ietf.org/rfc/rfc3629.txt

template <typename Iterator>
bool next(Iterator& pos, Iterator end) noexcept
{
    if(pos == end)
    {
        return false;
    }

    auto cu = static_cast<std::uint8_t>(*pos);

    for(auto const& [leading, trailing] : sequences)
    {
        if(cu > leading.max)
        {
            continue;
        }

        if(cu < leading.min)
        {
            break;
        }

        ++pos;

        for(auto const [min, max] : trailing)
        {
            if(min == 0)
            {
                break;
            }

            if(pos == end)
            {
                return false;
            }

            cu = static_cast<std::uint8_t>(*pos);

            if(cu < min || cu > max)
            {
                return false;
            }

            ++pos;
        }

        return true;
    }

    return false;
}

template <typename Iterator>
bool is_valid(Iterator first, Iterator last) noexcept
{
    while(first != last)
    {
        if(!next(first, last))
        {
            return false;
        }
    }

    return true;
}

inline std::size_t sequence_length(char32_t cp) noexcept
{
    if(cp < 0x80)
    {
        return 1;
    }

    if(cp < 0x800)
    {
        return 2;
    }

    if(cp < 0x10000)
    {
        return 3;
    }

    if(cp < 0x110000)
    {
        return 4;
    }

    return 0;
}

template <typename OutputOterator>
OutputOterator write(char32_t cp, OutputOterator out) noexcept
{
    std::array<std::uint8_t, 4> data;

    auto len = sequence_length(cp);
    auto pos = std::rbegin(data);

    switch(len)
    {
        case 4:
            *pos++ = static_cast<std::uint8_t>((cp & 0x3F) | 0x80), cp >>= 6;
            [[fallthrough]];

        case 3:
            *pos++ = static_cast<std::uint8_t>((cp & 0x3F) | 0x80), cp >>= 6;
            [[fallthrough]];

        case 2:
            *pos++ = static_cast<std::uint8_t>((cp & 0x3F) | 0x80), cp >>= 6;
            [[fallthrough]];

        case 1:
            *pos++ = static_cast<std::uint8_t>(cp | first_byte_base[len - 1]);
    }

    return std::copy_n(pos.base(), len, out);
}

inline bool is_valid(char32_t cp) noexcept
{
    std::array<std::uint8_t, 4> data;

    auto begin = std::begin(data);
    auto end = write(cp, begin);

    return next(begin, end);
}

} // namespace missio::utf8

#endif // _missio_json_utf8_hpp
