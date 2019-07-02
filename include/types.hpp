#ifndef _missio_json_types_hpp
#define _missio_json_types_hpp

#pragma once

#include <type_traits>
#include <string>
#include <vector>
#include <map>


namespace missio::json
{

enum class type
{
    null,
    true_,
    false_,
    integer,
    decimal,
    string,
    array,
    object
};

class value;

using boolean = bool;
using decimal = double;
using integer = long long;
using string  = std::string;
using array   = std::vector<value>;
using object  = std::map<string, value>;

inline constexpr type null = type::null;

using data = std::aligned_union_t<0, integer, decimal, string, array, object>;

} // namespace missio::json

namespace missio::json::impl
{

struct storage
{
    json::data data;
    json::type type;
};

} // namespace missio::json::impl

#endif // _missio_json_types_hpp
