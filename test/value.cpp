#include <doctest/doctest.h>

#include "json.hpp"

#include <iostream>
#include <fstream>


namespace json = missio::json;


struct Convertible {};

template <>
struct json::convert<Convertible>
{
    static value to_json(Convertible) { return {{"convertible", true}}; }
    static Convertible from_json(value) { return {}; }
};


template <typename T>
constexpr T pi = T(3.1415926535897932385);


void print_exception(std::exception const& e)
{
    std::cerr << e.what();

    try
    {
        std::rethrow_if_nested(e);
        std::cerr << '\n' << '\n';
    }
    catch(std::exception const& nested)
    {
        std::cerr << ':' << ' ';
        print_exception(nested);
    }
}

TEST_SUITE_BEGIN("value");

TEST_CASE("value test")
{
    {
        json::value value
        {
            {"int", 1},
            {"double", 2.0},
            {"custom", Convertible{}},
            {"array", json::array{1, 2.0, true}},
            {"object", {{"1", 1}, {"2", "two"}}}
        };

        value["int"] = 42;
        value["new"] = false;

        std::ofstream file{"object1.json"};

        file << value;
    }

    {
        json::value value;

        value["key"] = "value";
        value["pi"] = pi<double>;

        std::ofstream file{"object2.json"};

        file << value;
    }

    {
        json::value value = json::array{"one", 2, 3.0, false, 5U, 6ULL, 7.0F, 8.0L, Convertible{}, json::array{1, 2.0, true}, {{"object", true}}};

        value[0] = "two";
        value[12] = "new \"value\"\r\n\x1F";

        std::ofstream file{"array1.json"};

        file << value;
    }

    {
        json::value value;

        value[0] = json::array{1.0, 2.0, 3.0};
        value[1] = json::null;
        value[2] = "first";
        value[3] = true;
        value[4] = 42;

        std::ofstream file{"array2.json"};

        file << value;
    }

    {
        json::value value;

        std::ofstream file{"test.json"};
        file << std::boolalpha;

        file << value << std::endl;

        value = Convertible{};
        file << value << std::endl;

        value = "some very long string";
        file << value << std::endl;

        value = "other string";
        file << value << std::endl;

        value = false;
        file << value << std::endl;

        value = 1.0;
        file << value << std::endl;

        value = 421;
        file << value << std::endl;

        json::value moved = std::move(value);
        json::value copied = moved;

        file << std::endl;

        file << value << std::endl;
        file << moved << std::endl;
        file << copied << std::endl;
    }

    {
        std::string data = R"( { "string" : "string\t\r \u0462 \u9f98 \uD852\uDF62", "integer" : 42, "decimal" : 3.1415926535897932385, "boolean" : true, "null" : null, "array" : [ 0, 1.0, 0e1, false, null, {}, [] ] } )";

        json::value const value = json::parse(data);

        std::ofstream file{"parsed.json"};
        file << std::boolalpha;

        file << value << std::endl;

        file << std::endl;

        json::string const key{"array"};

        file << value[key][0] << " is decimal? " << value.at("array").at(0).is_decimal() << std::endl;
        file << value[key][1] << " is decimal? " << value.at("array").at(1).is_decimal() << std::endl;
        file << value[key][2] << " is decimal? " << value.at("array").at(2).is_decimal() << std::endl;
    }
}

TEST_SUITE_END();
