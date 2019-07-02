#ifndef _missio_json_error_hpp
#define _missio_json_error_hpp

#pragma once

#include "export.hpp"

#include <system_error>


namespace missio::json
{

enum class errc
{
    bad_boolean_cast            = 1,
    bad_number_cast             = 2,
    bad_string_cast             = 3,
    bad_array_cast              = 4,
    bad_object_cast             = 5,

    bad_decimal_number          = 6,
    bad_utf8_sequence           = 7,
    bad_utf16_code_unit         = 8,
    bad_utf32_code_point        = 9,
    bad_escape_sequence         = 10,
    bad_control_character       = 11,

    unexpected_utf16_surrogate  = 12,
    unexpected_leading_zeroes   = 13,
    unexpected_trailing_chars   = 14,
    unexpected_end_of_input     = 15,
    unexpected_trailing_text    = 16,

    expected_utf16_escape       = 17,
    expected_utf16_hex_digits   = 18,
    expected_utf16_surrogate    = 19,
    expected_decimal_digit      = 20,
    expected_exponent_digit     = 21,
    expected_valid_number       = 22,
    expected_valid_string       = 23,
    expected_true_literal       = 24,
    expected_false_literal      = 25,
    expected_null_literal       = 26,
    expected_array_close        = 27,
    expected_member_name        = 28,
    expected_member_name_sep    = 29,
    expected_member_value       = 30,
    expected_object_close       = 31,
    expected_string_close       = 32,

    cannot_parse_literal        = 33,
    cannot_parse_string         = 34,
    cannot_parse_object         = 35,
    cannot_parse_array          = 36,
    cannot_parse_number         = 37,
    cannot_parse_value          = 38,

    invalid_json_value          = 39,
    invalid_json_text           = 40
};

MISSIO_JSON_EXPORT std::error_category const& error_category() noexcept;

MISSIO_JSON_EXPORT std::error_code make_error_code(errc e) noexcept;

} // namespace missio::json

template <>
struct std::is_error_code_enum<::missio::json::errc> : std::true_type {};

#endif // _missio_json_error_hpp
