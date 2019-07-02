#include <doctest/doctest.h>

#include "value.hpp"
#include "parse.hpp"


namespace json = missio::json;


TEST_SUITE("parse")
{
    TEST_CASE("parse empty array")
    {
        json::value const value = json::parse("[]");

        REQUIRE(value.type() == json::type::array);
        CHECK(value.get_array().empty());
    }
}
