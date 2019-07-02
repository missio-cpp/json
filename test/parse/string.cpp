#include <doctest/doctest.h>

#include "value.hpp"
#include "parse.hpp"

#include <string>


namespace json = missio::json;

using namespace std::string_literals;


TEST_SUITE("parse")
{
    TEST_CASE("parse empty string")
    {
        json::value const value = json::parse("\"\"");

        REQUIRE(value.type() == json::type::string);
        CHECK(value.get_string() == ""s);
    }

    TEST_CASE("parse simple ASCII string")
    {
        json::value const value = json::parse("\"abcdef0123456\"");

        REQUIRE(value.type() == json::type::string);
        CHECK(value.get_string() == "abcdef0123456"s);
    }

    TEST_CASE("parse string containing all common escapes")
    {
        json::value const value = json::parse(R"("\"\/\\\b\f\n\r\t")");

        REQUIRE(value.type() == json::type::string);
        CHECK(value.get_string() == "\"/\\\b\f\n\r\t"s);
    }

    TEST_CASE("string containing single UTF-8 character")
    {
        json::value const value = json::parse(u8"\"\u9F98\"");

        REQUIRE(value.type() == json::type::string);
        CHECK(value.get_string() == "\xE9\xBE\x98"s);
    }

    TEST_CASE("parse string containing single Unicode escape")
    {
        json::value const value = json::parse(R"("\u0462")");

        REQUIRE(value.type() == json::type::string);
        CHECK(value.get_string() == "\xD1\xA2"s);
    }

    TEST_CASE("parse string containing escaped UTF-16 surrogate pair")
    {
        json::value const value = json::parse(R"("\uD852\uDF62")");

        REQUIRE(value.type() == json::type::string);
        CHECK(value.get_string() == "\xF0\xA4\xAD\xA2"s);
    }
}
