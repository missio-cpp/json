#include "write.hpp"
#include "value.hpp"
#include "error.hpp"

#include <initializer_list>
#include <system_error>
#include <algorithm>
#include <iterator>
#include <charconv>
#include <iostream>
#include <limits>


namespace missio::json::impl
{

#include "utils.inl"


// forward declaration
template <typename OutputIterator>
void write_value(OutputIterator out, value const& value);


template <typename OutputIterator, typename Range>
void write_range(OutputIterator out, Range const& range)
{
    std::copy(std::begin(range), std::end(range), out);
}

template <typename OutputIterator, typename... Chars>
void write_literal(OutputIterator out, Chars... chars)
{
    write_range(out, std::initializer_list<char>{chars...});
}

template <typename OutputIterator, typename Number>
void write_number(OutputIterator out, Number number)
{
    char str[std::numeric_limits<Number>::digits10 + 3];

    auto [ptr, err] = std::to_chars(std::begin(str), std::end(str), number);

    if(err != std::errc{})
    {
        throw std::system_error{std::make_error_code(err)};
    }

    std::copy(std::begin(str), ptr, out);
}

template <typename OutputIterator>
void write_utf16_char(OutputIterator out, unsigned char c)
{
    static constexpr char x[]
    {
        '0', '1', '2', '3', '4', '5', '6', '7',
        '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
    };

    write_literal(out, '\\', 'u', '0', '0');
    write_literal(out, x[c >> 4], x[c & 15]);
}

template <typename OutputIterator>
void write_control_char(OutputIterator out, char c)
{
    switch(c)
    {
        case '\b': 
            write_literal(out, '\\', 'b');
            break;

        case '\f':
            write_literal(out, '\\', 'f');
            break;

        case '\n':
            write_literal(out, '\\', 'n');
            break;

        case '\r':
            write_literal(out, '\\', 'r');
            break;

        case '\t':
            write_literal(out, '\\', 't');
            break;

        default:
            write_utf16_char(out, c);
    }
}

template <typename OutputIterator>
void write_common_char(OutputIterator out, char c)
{
    switch(c)
    {
        case '"':
        case '\\':
            write_literal(out, '\\');
            [[fallthrough]];

        default:
            write_literal(out, c);
    }
}

template <typename OutputIterator>
void write_string(OutputIterator out, string const& string)
{
    write_literal(out, '"');

    for(char const c : string)
    {
        if(is_control(c))
        {
            write_control_char(out, c);
        }
        else
        {
            write_common_char(out, c);
        }
    }

    write_literal(out, '"');
}

template <typename OutputIterator>
void write_array(OutputIterator out, array const& array)
{
    write_literal(out, '[');

    bool first = true;

    for(value const& value : array)
    {
        if(first)
        {
            first = false;
        }
        else
        {
            write_literal(out, ',');
        }

        write_value(out, value);
    }

    write_literal(out, ']');
}

template <typename OutputIterator>
void write_object(OutputIterator out, object const& object)
{
    write_literal(out, '{');

    bool first = true;

    for(auto const& [key, value] : object)
    {
        if(first)
        {
            first = false;
        }
        else
        {
            write_literal(out, ',');
        }

        write_string(out, key);
        write_literal(out, ':');
        write_value(out, value);
    }

    write_literal(out, '}');
}

template <typename OutputIterator>
void write_value(OutputIterator out, value const& value)
{
    switch(value.type())
    {
        case type::null:
            write_literal(out, 'n', 'u', 'l', 'l');
            break;

        case type::true_:
            write_literal(out, 't', 'r', 'u', 'e');
            break;

        case type::false_:
            write_literal(out, 'f', 'a', 'l', 's', 'e');
            break;

        case type::integer:
            write_number(out, value.get_integer());
            break;

        case type::decimal:
            write_number(out, value.get_decimal());
            break;

        case type::string:
            write_string(out, value.get_string());
            break;

        case type::array:
            write_array(out, value.get_array());
            break;

        case type::object:
            write_object(out, value.get_object());
            break;

        default:
            throw std::system_error{errc::invalid_json_value};
    }
}

} // namespace missio::json::impl


void missio::json::write(std::string& str, value const& value)
{
    impl::write_value(std::back_insert_iterator{str}, value);
}

void missio::json::write(std::ostream& os, value const& value)
{
    impl::write_value(std::ostreambuf_iterator{os}, value);
}
