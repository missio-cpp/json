#include <doctest/doctest.h>

#include "value.hpp"
#include "parse.hpp"

#include <system_error>


namespace json = missio::json;


TEST_SUITE("parse")
{
    TEST_CASE(R"(parse "null" literal)")
    {
        json::value const value = json::parse("null");

        REQUIRE(value.type() == json::type::null);
    }

    TEST_CASE(R"(parse invalid "null" literals)")
    {
        CHECK_THROWS_AS(json::parse("nul"), std::system_error);
        CHECK_THROWS_AS(json::parse("nULL"), std::system_error);
        CHECK_THROWS_AS(json::parse("nulll"), std::system_error);
    }

    TEST_CASE(R"(parse "true" literal)")
    {
        json::value const value = json::parse("true");

        REQUIRE(value.type() == json::type::true_);
    }

    TEST_CASE(R"(parse invalid "true" literals)")
    {
        CHECK_THROWS_AS(json::parse("tru"), std::system_error);
        CHECK_THROWS_AS(json::parse("tRUE"), std::system_error);
        CHECK_THROWS_AS(json::parse("truee"), std::system_error);
    }

    TEST_CASE(R"(parse "false" literal)")
    {
        json::value const value = json::parse("false");

        REQUIRE(value.type() == json::type::false_);
    }

    TEST_CASE(R"(parse invalid "false" literals)")
    {
        CHECK_THROWS_AS(json::parse("fals"), std::system_error);
        CHECK_THROWS_AS(json::parse("fALSE"), std::system_error);
        CHECK_THROWS_AS(json::parse("falsee"), std::system_error);
    }
}
