#include <doctest/doctest.h>

#include "value.hpp"
#include "parse.hpp"


namespace json = missio::json;


TEST_SUITE("parse")
{
    TEST_CASE("parse empty object")
    {
        json::value const value = json::parse("{}");

        REQUIRE(value.type() == json::type::object);
        CHECK(value.get_object().empty());
    }
}
