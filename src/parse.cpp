#include "parse.hpp"
#include "value.hpp"
#include "error.hpp"
#include "utils.hpp"
#include "utf16.hpp"
#include "utf8.hpp"

#include <initializer_list>
#include <system_error>
#include <algorithm>
#include <functional>
#include <cstdint>
#include <iterator>
#include <iostream>
#include <charconv>


namespace missio::json::impl
{

#include "utils.inl"


// forward declaration
template <typename Iterator>
bool parse_value(Iterator& first, Iterator last, value& out);


template <typename Iterator>
std::string make_what_arg(Iterator first, Iterator last, Iterator current)
{
    first = std::find_if(first, current, std::not_fn(is_space));
    last = std::find_if(current, last, is_delimiter);

    return {first, last};
}

template <typename Iterator>
std::string make_what_arg(Iterator first, Iterator last)
{
    return make_what_arg(first, last, first);
}


template <char Char, typename Iterator>
bool parse_one(Iterator& first, Iterator last)
{
    if(first == last)
    {
        return false;
    }

    if(Char != *first)
    {
        return false;
    }

    ++first;

    return true;
}

template <char... Chars, typename Iterator>
bool parse_one_of(Iterator& first, Iterator last)
{
    return (parse_one<Chars>(first, last) || ...);
}

template <char... Chars, typename Iterator>
bool parse(Iterator& first, Iterator last)
{
    Iterator current{first};

    if(!(parse_one<Chars>(current, last) && ...))
    {
        return false;
    }

    first = current;

    return true;
}

template <typename Iterator, typename Predicate>
bool parse_one_if(Iterator& first, Iterator last, Predicate pred)
{
    if(first == last)
    {
        return false;
    }

    if(!pred(*first))
    {
        return false;
    }

    ++first;

    return true;
}

template <typename Iterator, typename Predicate>
std::size_t parse_if(Iterator& first, Iterator last, Predicate pred)
{
    std::size_t n = 0;

    while(parse_one_if(first, last, pred)) ++n;

    return n;
}


template <char... Chars, typename Iterator>
bool match(Iterator first, Iterator last)
{
    return parse<Chars...>(first, last);
}

template <typename Iterator, typename Predicate>
bool match_one_if(Iterator first, Iterator last, Predicate pred)
{
    return parse_one_if(first, last, pred);
}


template <typename Iterator>
void skip_whitespace(Iterator& first, Iterator last)
{
    while(parse_one_if(first, last, is_space));
}


template <typename Iterator>
bool parse_utf16(Iterator& first, Iterator last, char16_t& out)
{
    Iterator current{first};

    if(!parse<'\\', 'u'>(current, last))
    {
        return false;
    }

    try
    {
        char str[4];

        if(std::distance(current, last) < ssize(str))
        {
            throw std::system_error{errc::unexpected_end_of_input};
        }

        std::copy_n(current, std::size(str), std::begin(str));

        std::advance(current, std::size(str));

        if(!std::all_of(std::begin(str), std::end(str), is_xdigit))
        {
            throw std::system_error{errc::expected_utf16_hex_digits, make_what_arg(first, last, current)};
        }

        std::uint16_t val;

        auto [ptr, err] = std::from_chars(std::begin(str), std::end(str), val, 16);

        if(err != std::errc{})
        {
            throw std::system_error{std::make_error_code(err), make_what_arg(first, last, current)};
        }

        out = static_cast<char16_t>(val);
    }
    catch(...)
    {
        std::throw_with_nested(std::system_error{errc::bad_utf16_code_unit});
    }

    first = current;

    return true;
}

template <typename Iterator, typename OutputIterator>
bool parse_utf16_escape(Iterator& first, Iterator last, OutputIterator out)
{
    char16_t cu;

    Iterator current{first};

    if(!parse_utf16(current, last, cu))
    {
        return false;
    }

    if(utf16::is_low_surrogate(cu))
    {
        throw std::system_error{errc::unexpected_utf16_surrogate, make_what_arg(first, current)};
    }

    char32_t cp;

    if(utf16::is_high_surrogate(cu))
    {
        char16_t next_cu;

        if(!parse_utf16(current, last, next_cu))
        {
            throw std::system_error{errc::expected_utf16_surrogate, make_what_arg(first, last)};
        }

        if(!utf16::is_low_surrogate(next_cu))
        {
            throw std::system_error{errc::expected_utf16_surrogate, make_what_arg(first, current)};
        }

        cp = utf16::make_utf32(cu, next_cu);
    }
    else
    {
        cp = cu;
    }

    if(!utf8::is_valid(cp))
    {
        throw std::system_error{errc::bad_utf32_code_point, make_what_arg(first, current)};
    }

    utf8::write(cp, out);

    first = current;

    return true;
}

template <typename Iterator, typename OutputIterator>
bool parse_common_escape(Iterator& first, Iterator last, OutputIterator out)
{
    Iterator current{first};

    if(!parse<'\\'>(current, last))
    {
        return false;
    }

    if(current == last)
    {
        throw std::system_error{errc::unexpected_end_of_input};
    }

    switch(*current)
    {
        case '"': *out = '"'; break;
        case '/': *out = '/'; break;
        case 'b': *out = '\b'; break;
        case 'f': *out = '\f'; break;
        case 'n': *out = '\n'; break;
        case 'r': *out = '\r'; break;
        case 't': *out = '\t'; break;
        case '\\': *out = '\\'; break;

        default:
            throw std::system_error{errc::bad_escape_sequence, make_what_arg(first, last)};
    }

    ++out;

    ++current;

    first = current;

    return true;
}

template <typename Iterator, typename OutputIterator>
bool parse_escaped_char(Iterator& first, Iterator last, OutputIterator out)
{
    if(parse_utf16_escape(first, last, out))
    {
        return true;
    }

    if(parse_common_escape(first, last, out))
    {
        return true;
    }

    return false;
}

template <typename Iterator, typename OutputIterator>
void parse_common_char(Iterator& first, Iterator last, OutputIterator out)
{
    Iterator current{first};

    if(current == last)
    {
        throw std::system_error{errc::unexpected_end_of_input};
    }

    if(is_control(*current))
    {
        throw std::system_error{errc::bad_control_character, make_what_arg(first, last)};
    }

    if(!utf8::next(current, last))
    {
        throw std::system_error{errc::bad_utf8_sequence, make_what_arg(first, last)};
    }

    std::copy(first, current, out);

    first = current;
}

template <typename Iterator>
bool parse_string(Iterator& first, Iterator last, string& out)
{
    Iterator current{first};

    if(!parse<'"'>(current, last))
    {
        return false;
    }

    try
    {
        std::back_insert_iterator inserter{out};

        while(!parse<'"'>(current, last))
        {
            if(!parse_escaped_char(current, last, inserter))
            {
                parse_common_char(current, last, inserter);
            }
        }

        if(match_one_if(current, last, std::not_fn(is_delimiter)))
        {
            throw std::system_error{errc::unexpected_trailing_chars, make_what_arg(first, last, current)};
        }
    }
    catch(...)
    {
        std::throw_with_nested(std::system_error{errc::cannot_parse_string});
    }

    first = current;

    return true;
}


template <typename Iterator>
bool parse_object(Iterator& first, Iterator last, value& out)
{
    if(!parse<'{'>(first, last))
    {
        return false;
    }

    try
    {
        object& object = out.convert_to_object();

        skip_whitespace(first, last);

        if(parse<'}'>(first, last))
        {
            return true;
        }

        do
        {
            string name;

            Iterator current{first};

            skip_whitespace(current, last);

            if(!parse_string(current, last, name))
            {
                throw std::system_error{errc::expected_member_name, make_what_arg(first, last, current)};
            }

            skip_whitespace(current, last);

            if(!parse<':'>(current, last))
            {
                throw std::system_error{errc::expected_member_name_sep, make_what_arg(first, last, current)};
            }

            value value;

            skip_whitespace(current, last);

            if(!parse_value(current, last, value))
            {
                throw std::system_error{errc::expected_member_value, make_what_arg(first, last, current)};
            }

            object.insert_or_assign(std::move(name), std::move(value));

            skip_whitespace(current, last);

            first = current;
        }
        while(parse<','>(first, last));

        if(!parse<'}'>(first, last))
        {
            throw std::system_error{errc::expected_object_close, make_what_arg(first, last)};
        }
    }
    catch(...)
    {
        std::throw_with_nested(std::system_error{errc::cannot_parse_object});
    }

    return true;
}

template <typename Iterator>
bool parse_array(Iterator& first, Iterator last, value& out)
{
    if(!parse<'['>(first, last))
    {
        return false;
    }

    try
    {
        array& array = out.convert_to_array();

        skip_whitespace(first, last);

        if(parse<']'>(first, last))
        {
            return true;
        }

        do
        {
            value value;

            Iterator current{first};

            skip_whitespace(current, last);

            if(!parse_value(current, last, value))
            {
                throw std::system_error{errc::expected_member_value, make_what_arg(first, last, current)};
            }

            array.push_back(std::move(value));

            skip_whitespace(current, last);

            first = current;
        }
        while(parse<','>(first, last));

        if(!parse<']'>(first, last))
        {
            throw std::system_error{errc::expected_array_close, make_what_arg(first, last)};
        }
    }
    catch(...)
    {
        std::throw_with_nested(std::system_error{errc::cannot_parse_array});
    }

    return true;
}

template <typename Iterator>
bool parse_string(Iterator& first, Iterator last, value& out)
{
    Iterator current{first};

    if(!parse<'"'>(current, last))
    {
        return false;
    }

    try
    {
        string string;

        std::back_insert_iterator inserter{string};

        while(!parse<'"'>(current, last))
        {
            if(!parse_escaped_char(current, last, inserter))
            {
                parse_common_char(current, last, inserter);
            }
        }

        if(match_one_if(current, last, std::not_fn(is_delimiter)))
        {
            throw std::system_error{errc::unexpected_trailing_chars, make_what_arg(first, last, current)};
        }

        out = std::move(string);
    }
    catch(...)
    {
        std::throw_with_nested(std::system_error{errc::cannot_parse_string});
    }

    first = current;

    return true;
}

template <typename Number, typename Iterator>
void parse_number(Iterator first, Iterator last, value& out)
{
    Number number;

    auto [ptr, err] = std::from_chars(first, last, number);

    if(err != std::errc{})
    {
        throw std::system_error{std::make_error_code(err), make_what_arg(first, last)};
    }

    out = number;
}

template <typename Iterator>
bool parse_number(Iterator& first, Iterator last, value& out)
{
    Iterator current{first};

    try
    {
        parse<'-'>(current, last);

        if(parse<'0'>(current, last))
        {
            if(match_one_if(current, last, is_digit))
            {
                throw std::system_error{errc::unexpected_leading_zeroes, make_what_arg(first, last)};
            }
        }
        else if(!parse_if(current, last, is_digit))
        {
            return false;
        }

        bool is_integer = true;

        if(parse<'.'>(current, last))
        {
            if(!parse_if(current, last, is_digit))
            {
                throw std::system_error{errc::expected_decimal_digit, make_what_arg(first, last)};
            }

            is_integer = false;
        }

        if(parse_one_of<'e', 'E'>(current, last))
        {
            parse_one_of<'-', '+'>(current, last);

            if(!parse_if(current, last, is_digit))
            {
                throw std::system_error{errc::expected_exponent_digit, make_what_arg(first, last)};
            }

            is_integer = false;
        }

        if(match_one_if(current, last, std::not_fn(is_delimiter)))
        {
            throw std::system_error{errc::unexpected_trailing_chars, make_what_arg(first, last)};
        }

        char data[32];

        if(std::distance(first, current) > ssize(data))
        {
            throw std::system_error{errc::expected_valid_number, make_what_arg(first, last)};
        }

        auto const end = std::copy(first, current, std::begin(data));

        if(is_integer)
        {
            parse_number<integer>(std::begin(data), end, out);
        }
        else
        {
            parse_number<decimal>(std::begin(data), end, out);
        }
    }
    catch(...)
    {
        std::throw_with_nested(std::system_error{errc::cannot_parse_number});
    }

    first = current;

    return true;
}

template <typename Iterator>
bool parse_true(Iterator& first, Iterator last, value& out)
{
    if(!match<'t'>(first, last))
    {
        return false;
    }

    try
    {
        Iterator current{first};

        if(!parse<'t', 'r', 'u', 'e'>(current, last))
        {
            throw std::system_error{errc::expected_true_literal, make_what_arg(first, last)};
        }

        if(match_one_if(current, last, std::not_fn(is_delimiter)))
        {
            throw std::system_error{errc::unexpected_trailing_chars, make_what_arg(first, last)};
        }

        first = current;
    }
    catch(...)
    {
        std::throw_with_nested(std::system_error{errc::cannot_parse_literal});
    }

    out = type::true_;

    return true;
}

template <typename Iterator>
bool parse_false(Iterator& first, Iterator last, value& out)
{
    if(!match<'f'>(first, last))
    {
        return false;
    }

    try
    {
        Iterator current{first};

        if(!parse<'f', 'a', 'l', 's', 'e'>(current, last))
        {
            throw std::system_error{errc::expected_false_literal, make_what_arg(first, last)};
        }

        if(match_one_if(current, last, std::not_fn(is_delimiter)))
        {
            throw std::system_error{errc::unexpected_trailing_chars, make_what_arg(first, last)};
        }

        first = current;
    }
    catch(...)
    {
        std::throw_with_nested(std::system_error{errc::cannot_parse_literal});
    }

    out = type::false_;

    return true;
}

template <typename Iterator>
bool parse_null(Iterator& first, Iterator last, value& out)
{
    if(!match<'n'>(first, last))
    {
        return false;
    }

    try
    {
        Iterator current{first};

        if(!parse<'n', 'u', 'l', 'l'>(current, last))
        {
            throw std::system_error{errc::expected_null_literal, make_what_arg(first, last)};
        }

        if(match_one_if(current, last, std::not_fn(is_delimiter)))
        {
            throw std::system_error{errc::unexpected_trailing_chars, make_what_arg(first, last)};
        }

        first = current;
    }
    catch(...)
    {
        std::throw_with_nested(std::system_error{errc::cannot_parse_literal});
    }

    out = type::null;

    return true;
}

template <typename Iterator>
bool parse_value(Iterator& first, Iterator last, value& out)
{
    if(first == last)
    {
        throw std::system_error{errc::unexpected_end_of_input};
    }

    return parse_object(first, last, out)
        || parse_array(first, last, out)
        || parse_string(first, last, out)
        || parse_number(first, last, out)
        || parse_true(first, last, out)
        || parse_false(first, last, out)
        || parse_null(first, last, out);
}


template <typename Iterator>
value parse(Iterator first, Iterator last)
{
    try
    {
        value value;

        skip_whitespace(first, last);

        if(!parse_value(first, last, value))
        {
            throw std::system_error{errc::cannot_parse_value, make_what_arg(first, last)};
        }

        skip_whitespace(first, last);

        if(first != last)
        {
            throw std::system_error{errc::unexpected_trailing_text, make_what_arg(first, last)};
        }

        return value;
    }
    catch(...)
    {
        std::throw_with_nested(std::system_error{errc::invalid_json_text});
    }
}

} // namespace json::impl


missio::json::value missio::json::parse(std::string_view str)
{
    return impl::parse(std::begin(str), std::end(str));
}

missio::json::value missio::json::parse(std::istream& is)
{
    return impl::parse(std::istreambuf_iterator{is}, {});
}
