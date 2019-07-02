#include <doctest/doctest.h>

#include "value.hpp"
#include "write.hpp"

#include <string>


namespace json = missio::json;

using namespace std::string_literals;


TEST_SUITE("write")
{
    TEST_CASE(R"(write "null" literal)")
    {
        std::string result;
        json::value value{json::type::null};
        json::write(result, value);

        REQUIRE(result == "null"s);
    }

    TEST_CASE(R"(write "true" literal)")
    {
        std::string result;
        json::value value{json::type::true_};
        json::write(result, value);

        REQUIRE(result == "true"s);
    }

    TEST_CASE(R"(write "false" literal)")
    {
        std::string result;
        json::value value{json::type::false_};
        json::write(result, value);

        REQUIRE(result == "false"s);
    }
}
