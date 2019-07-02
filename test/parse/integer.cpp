#include <doctest/doctest.h>

#include "value.hpp"
#include "parse.hpp"
#include "error.hpp"


namespace json = missio::json;


TEST_SUITE("parse")
{
    TEST_CASE("parse positive zero integer")
    {
        json::value const value = json::parse("0");

        REQUIRE(value.type() == json::type::integer);
        CHECK(value.get_integer() == 0LL);
    }

    TEST_CASE("parse negative zero integer")
    {
        json::value const value = json::parse("-0");

        REQUIRE(value.type() == json::type::integer);
        CHECK(value.get_integer() == 0LL);
    }

    TEST_CASE("parse positive integer")
    {
        json::value const value = json::parse("123456");

        REQUIRE(value.type() == json::type::integer);
        CHECK(value.get_integer() == 123456LL);
    }

    TEST_CASE("parse negative integer")
    {
        json::value const value = json::parse("-123456");

        REQUIRE(value.type() == json::type::integer);
        CHECK(value.get_integer() == -123456LL);
    }

    TEST_CASE("parse invalid negative integer")
    {
        CHECK_THROWS_AS(json::parse("-"), std::system_error);
        CHECK_THROWS_AS(json::parse("-A"), std::system_error);
        CHECK_THROWS_AS(json::parse("-1A"), std::system_error);
        CHECK_THROWS_AS(json::parse("- 1"), std::system_error);
    }

    TEST_CASE("parse invalid positive integer")
    {
        CHECK_THROWS_AS(json::parse("+"), std::system_error);
        CHECK_THROWS_AS(json::parse("+0"), std::system_error);
        CHECK_THROWS_AS(json::parse("+1"), std::system_error);
        CHECK_THROWS_AS(json::parse("1A"), std::system_error);
    }
}
