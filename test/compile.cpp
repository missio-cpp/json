#include "json.hpp"

#include <unordered_set>
#include <unordered_map>
#include <forward_list>
#include <string_view>
#include <string>
#include <vector>
#include <deque>
#include <array>
#include <list>
#include <set>
#include <map>


namespace json = missio::json;


static_assert(json::traits::is_sequence_container_v<std::forward_list<double>>);
static_assert(json::traits::is_sequence_container_v<std::vector<size_t>>);
static_assert(json::traits::is_sequence_container_v<std::array<float, 5>>);
static_assert(json::traits::is_sequence_container_v<std::deque<bool>>);
static_assert(json::traits::is_sequence_container_v<std::set<std::string>>);
static_assert(json::traits::is_sequence_container_v<std::multiset<std::string>>);
static_assert(json::traits::is_sequence_container_v<std::unordered_set<bool>>);
static_assert(json::traits::is_sequence_container_v<std::set<json::value>>);

static_assert(!json::traits::is_associative_container_v<std::forward_list<std::string>>);
static_assert(!json::traits::is_associative_container_v<std::vector<size_t>>);
static_assert(!json::traits::is_associative_container_v<std::array<float, 5>>);
static_assert(!json::traits::is_associative_container_v<std::set<std::string>>);
static_assert(!json::traits::is_associative_container_v<std::multiset<std::string>>);
static_assert(!json::traits::is_associative_container_v<std::unordered_set<bool>>);
static_assert(!json::traits::is_associative_container_v<std::vector<json::value>>);
static_assert(!json::traits::is_associative_container_v<std::set<json::value>>);

static_assert(json::traits::is_associative_container_v<std::map<std::string, size_t>>);
static_assert(json::traits::is_associative_container_v<std::map<std::string_view, std::string>>);
static_assert(json::traits::is_associative_container_v<std::unordered_map<std::string, size_t>>);
static_assert(json::traits::is_associative_container_v<std::unordered_map<std::string, json::value>>);
static_assert(json::traits::is_associative_container_v<std::multimap<std::string, json::value>>);

static_assert(!json::traits::is_sequence_container_v<std::map<std::string, size_t>>);
static_assert(!json::traits::is_sequence_container_v<std::unordered_map<std::string, size_t>>);
static_assert(!json::traits::is_sequence_container_v<std::map<json::value, std::string>>);
static_assert(!json::traits::is_sequence_container_v<std::map<std::string, json::value>>);
static_assert(!json::traits::is_sequence_container_v<std::map<json::value, json::value>>);
static_assert(!json::traits::is_sequence_container_v<std::map<std::string, std::string>>);
static_assert(!json::traits::is_associative_container_v<std::map<size_t, std::string>>);

struct Struct {};

static_assert(!json::traits::is_sequence_container_v<std::vector<Struct>>);
static_assert(!json::traits::is_associative_container_v<std::map<Struct, std::string>>);
static_assert(!json::traits::is_associative_container_v<std::map<std::string, Struct>>);

enum class Enum { one, two, three };

static_assert(json::traits::is_sequence_container_v<std::set<Enum>>);
static_assert(json::traits::is_sequence_container_v<std::vector<Enum>>);
static_assert(json::traits::is_associative_container_v<std::map<std::string, Enum>>);

static_assert(!json::traits::is_associative_container_v<std::map<Enum, std::string>>);


struct Convertible {};

template <>
struct json::convert<Convertible>
{
    static value to_json(Convertible) { return {}; }
    static Convertible from_json(value) { return {}; }
};

static_assert(json::traits::is_convertible_to_json_v<Convertible>);
static_assert(json::traits::is_convertible_from_json_v<Convertible>);

static_assert(json::traits::is_sequence_container_v<std::vector<Convertible>>);
static_assert(json::traits::is_associative_container_v<std::map<std::string, Convertible>>);

static_assert(!json::traits::is_associative_container_v<std::map<Convertible, json::value>>);

struct NotConvertible {};

static_assert(!json::traits::is_convertible_to_json_v<NotConvertible>);
static_assert(!json::traits::is_convertible_from_json_v<NotConvertible>);

static_assert(!json::traits::is_sequence_container_v<std::vector<NotConvertible>>);
static_assert(!json::traits::is_associative_container_v<std::map<std::string, NotConvertible>>);


void test()
{
    json::value value;

    value = std::int8_t{1};
    value = std::int16_t{2};
    value = std::int32_t{3};
    value = std::int64_t{4};

    value = std::uint8_t{1};
    value = std::uint16_t{2};
    value = std::uint32_t{3};
    value = std::uint64_t{4};

    value = static_cast<float>(1);
    value = static_cast<double>(2);
    value = static_cast<long double>(3);

    value = json::array{"one", 2, 3.0, false, 5U, 6ULL, 7.0F, 8.0L, Convertible{}, json::array{1, 2.0, true}, {{"object", true}}};

    value[0] = "two";
    value.at(0) = "two";

    value = {{"int", 1}, {"double", 2.0}, {"convertible", Convertible{}}, {"array", json::array{1, 2.0, true}}, {"object", {{"1", 1}, {"2", "two"}}}};

    value["int"] = 42;
    value.at("int") = 42;
    value[std::string{"int"}] = 42;

    value = 42;
    int i = value;
    static_cast<void>(i);

    value = 42u;
    unsigned u = value;
    static_cast<void>(u);

    value = 1.0;
    double d = value;
    static_cast<void>(d);

    value = 1.0f;
    float f = value;
    static_cast<void>(f);

    value = Enum::one;
    Enum e = value;
    static_cast<void>(e);

    value = true;
    bool b = value;
    static_cast<void>(b);

    char const* str_literal{"Hello, world!"};
    value = str_literal;

    char const str_array[]{"Hello, world!"};
    value = str_array;

    value = std::string{"Hello, world!"};
    std::string std_string = value;
    static_cast<void>(std_string);

    value = std::list<int>{1, 2, 3};
    std::list<int> int_list = value;
    static_cast<void>(int_list);

    double const double_array[]{1.0, 2.0, 3.0};
    value = double_array;

    value = std::set<std::string>{"1", "2", "3"};
    std::set<std::string> str_set = value;
    static_cast<void>(str_set);

    value = std::vector<std::string>{"1", "2", "3"};
    std::vector<std::string> str_vector = value;
    static_cast<void>(str_vector);

    value = std::vector<Enum>{Enum::one, Enum::two};
    std::vector<Enum> enum_vector = value;
    static_cast<void>(enum_vector);

    value = std::deque<std::string>{"one", "two", "three"};
    std::deque<std::string> str_deque = value;
    static_cast<void>(str_deque);

    value = json::array{1, "2", 3.0, false};
    json::array& json_array = value;
    static_cast<void>(json_array);

    value = json::object{{"one", 1}, {"two", 2.0}};
    json::object& json_object = value;
    static_cast<void>(json_object);

    value = std::map<std::string, size_t>{{"1", 1}, {"2", 2}};
    std::map<std::string, size_t> map = value;
    static_cast<void>(map);

    value = Convertible{};
    Convertible convertible = value;
    static_cast<void>(convertible);
}
