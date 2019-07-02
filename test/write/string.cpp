#include <doctest/doctest.h>

#include "value.hpp"
#include "write.hpp"

#include <string>


namespace json = missio::json;

using namespace std::string_literals;


TEST_SUITE("write")
{
    TEST_CASE("write empty string")
    {
        std::string result;
        json::value value{json::type::string};
        json::write(result, value);

        REQUIRE(result == R"("")"s);
    }

    TEST_CASE(R"(write ASCII string)")
    {
        std::string result;
        json::value value{"abcABC123"};
        json::write(result, value);

        REQUIRE(result ==  R"("abcABC123")"s);
    }

    TEST_CASE(R"(write BMP character UTF-8 string)")
    {
        std::string result;
        json::value value{""};
        json::write(result, value);

        REQUIRE(result ==  R"("")"s);
    }

    TEST_CASE(R"(write non-BMP character UTF-8 string)")
    {
        std::string result;
        json::value value{""};
        json::write(result, value);

        REQUIRE(result ==  R"("")"s);
    }
}
