#include <doctest/doctest.h>

#include "value.hpp"
#include "parse.hpp"
#include "error.hpp"


namespace json = missio::json;


TEST_SUITE("parse")
{
    TEST_CASE("parse positive zero decimal number")
    {
        json::value const value = json::parse("0.0");

        REQUIRE(value.type() == json::type::decimal);
        CHECK(value.get_decimal() == 0.0);
    }

    TEST_CASE("parse negative zero decimal number")
    {
        json::value const value = json::parse("-0.0");

        REQUIRE(value.type() == json::type::decimal);
        CHECK(value.get_decimal() == 0.0);
    }

    TEST_CASE("parse positive decimal number")
    {
        json::value const value = json::parse("3.141592");

        REQUIRE(value.type() == json::type::decimal);
        CHECK(value.get_decimal() == 3.141592);
    }

    TEST_CASE("parse negative decimal number")
    {
        json::value const value = json::parse("-3.141592");

        REQUIRE(value.type() == json::type::decimal);
        CHECK(value.get_decimal() == -3.141592);
    }

    TEST_CASE("parse invalid positive zero decimal number")
    {
        CHECK_THROWS_AS(json::parse("0."), std::system_error);
        CHECK_THROWS_AS(json::parse("0. "), std::system_error);
        CHECK_THROWS_AS(json::parse("0.A"), std::system_error);
        CHECK_THROWS_AS(json::parse("00.0"), std::system_error);
        CHECK_THROWS_AS(json::parse("0.0A"), std::system_error);
        CHECK_THROWS_AS(json::parse("0. 0"), std::system_error);
        CHECK_THROWS_AS(json::parse("0 .0"), std::system_error);
        CHECK_THROWS_AS(json::parse("0,0"), std::system_error);
        CHECK_THROWS_AS(json::parse("0.0,0"), std::system_error);
        CHECK_THROWS_AS(json::parse("0.0 0"), std::system_error);
    }

    TEST_CASE("parse invalid negative zero decimal number")
    {
        CHECK_THROWS_AS(json::parse("-0."), std::system_error);
        CHECK_THROWS_AS(json::parse("-0. "), std::system_error);
        CHECK_THROWS_AS(json::parse("-0.A"), std::system_error);
        CHECK_THROWS_AS(json::parse("-00.0"), std::system_error);
        CHECK_THROWS_AS(json::parse("-0.0A"), std::system_error);
        CHECK_THROWS_AS(json::parse("-A.0"), std::system_error);
        CHECK_THROWS_AS(json::parse("- 0.0"), std::system_error);
        CHECK_THROWS_AS(json::parse("-0,0"), std::system_error);
        CHECK_THROWS_AS(json::parse("-0.0,0"), std::system_error);
        CHECK_THROWS_AS(json::parse("-0.0 0"), std::system_error);
    }

    TEST_CASE("parse invalid negative decimal number")
    {
        CHECK_THROWS_AS(json::parse("-1."), std::system_error);
        CHECK_THROWS_AS(json::parse("-1.A"), std::system_error);
        CHECK_THROWS_AS(json::parse("-1.0A"), std::system_error);
        CHECK_THROWS_AS(json::parse("-01.0"), std::system_error);
        CHECK_THROWS_AS(json::parse("-1..0"), std::system_error);
        CHECK_THROWS_AS(json::parse("-1.0.0"), std::system_error);
        CHECK_THROWS_AS(json::parse("-1.0,0"), std::system_error);
        CHECK_THROWS_AS(json::parse("-1.0 0"), std::system_error);
        CHECK_THROWS_AS(json::parse("-1,0"), std::system_error);
        CHECK_THROWS_AS(json::parse("-1 ,0"), std::system_error);
        CHECK_THROWS_AS(json::parse("-1, 0"), std::system_error);
        CHECK_THROWS_AS(json::parse("- 1.0"), std::system_error);
    }

    TEST_CASE("parse invalid positive decimal number")
    {
        CHECK_THROWS_AS(json::parse("+1."), std::system_error);
        CHECK_THROWS_AS(json::parse("+1.A"), std::system_error);
        CHECK_THROWS_AS(json::parse("1.A"), std::system_error);
        CHECK_THROWS_AS(json::parse("1.0A"), std::system_error);
        CHECK_THROWS_AS(json::parse("1,0"), std::system_error);
        CHECK_THROWS_AS(json::parse("1..0"), std::system_error);
        CHECK_THROWS_AS(json::parse("1.0.0"), std::system_error);
        CHECK_THROWS_AS(json::parse("1.0,0"), std::system_error);
        CHECK_THROWS_AS(json::parse("1.0 0"), std::system_error);
        CHECK_THROWS_AS(json::parse("1 .0"), std::system_error);
        CHECK_THROWS_AS(json::parse("1. 0"), std::system_error);
    }
}
