#ifndef _missio_json_parse_hpp
#define _missio_json_parse_hpp

#pragma once

#include "export.hpp"

#include <string_view>
#include <iosfwd>


namespace missio::json
{

class value;

MISSIO_JSON_EXPORT value parse(std::string_view str);
MISSIO_JSON_EXPORT value parse(std::istream& is);

} // namespace missio::json

#endif // _missio_json_parse_hpp
