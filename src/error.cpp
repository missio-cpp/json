#include "error.hpp"


namespace missio::json::detail
{

class error_category final : public std::error_category
{
public:
    [[nodiscard]] char const* name() const noexcept override
    {
        return "json";
    }

    [[nodiscard]] std::string message(int e) const override
    {
        switch(static_cast<errc>(e))
        {
            case errc::bad_boolean_cast:
                return R"(value cannot be cast to a boolean)";

            case errc::bad_number_cast:
                return R"(value cannot be cast to a number)";

            case errc::bad_string_cast:
                return R"(value cannot be cast to a string)";

            case errc::bad_array_cast:
                return R"(value is not an array)";

            case errc::bad_object_cast:
                return R"(value is not an object)";

            case errc::bad_decimal_number:
                return R"(floating point number is not finite)";

            case errc::bad_utf8_sequence:
                return R"(invalid UTF-8 sequence)";

            case errc::bad_utf16_code_unit:
                return R"(invalid UTF-16 code unit)";

            case errc::bad_utf32_code_point:
                return R"(invalid UTF-32 code point)";

            case errc::bad_escape_sequence:
                return R"(illegal escape sequence)";

            case errc::bad_control_character:
                return R"(illegal control character)";

            case errc::unexpected_utf16_surrogate:
                return R"(unexpected UTF-16 low surrogate)";

            case errc::unexpected_leading_zeroes:
                return R"(leading zeroes are not permitted)";

            case errc::unexpected_trailing_chars:
                return R"(unexpected trailing characters)";

            case errc::unexpected_end_of_input:
                return R"(unexpected end of input)";

            case errc::unexpected_trailing_text:
                return R"(unexpected non-whitespace trailing characters)";

            case errc::expected_utf16_escape:
                return R"(expected "\u" literal starting UTF-16 escape)";

            case errc::expected_utf16_hex_digits:
                return R"(expected four hexadecimal digits in UTF-16 escape)";

            case errc::expected_utf16_surrogate:
                return R"(expected low surrogate after UTF-16 high surrogate)";

            case errc::expected_decimal_digit:
                return R"(expected at least one digit after decimal point)";

            case errc::expected_exponent_digit:
                return R"(expected at least one digit after exponent)";

            case errc::expected_valid_number:
                return R"(expected valid integral or decimal number)";

            case errc::expected_valid_string:
                return R"(expected valid string literal)";

            case errc::expected_true_literal:
                return R"(expected "true" literal)";

            case errc::expected_false_literal:
                return R"(expected "false" literal)";

            case errc::expected_null_literal:
                return R"(expected "null" literal)";

            case errc::expected_array_close:
                return R"(expected comma or ']' character)";

            case errc::expected_member_name:
                return R"(expected member name)";

            case errc::expected_member_name_sep:
                return R"(expected ':' character after member name)";

            case errc::expected_member_value:
                return R"(expected member value)";

            case errc::expected_object_close:
                return R"(expected comma or '}' character)";

            case errc::expected_string_close:
                return R"(expected '"' character)";

            case errc::cannot_parse_literal:
                return R"(cannot parse literal)";

            case errc::cannot_parse_string:
                return R"(cannot parse string)";

            case errc::cannot_parse_object:
                return R"(cannot parse object)";

            case errc::cannot_parse_array:
                return R"(cannot parse array)";

            case errc::cannot_parse_number:
                return R"(cannot parse number)";

            case errc::cannot_parse_value:
                return R"(cannot parse value)";

            case errc::invalid_json_value:
                return R"(invalid JSON value)";

            case errc::invalid_json_text:
                return R"(invalid JSON text)";
        }

        return R"(unknown error)";
    }
};

} // namespace json::detail

std::error_category const& missio::json::error_category() noexcept
{
    static detail::error_category error_category;
    return error_category;
}

std::error_code missio::json::make_error_code(errc e) noexcept
{
    return {static_cast<int>(e), error_category()};
}
