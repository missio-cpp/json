#ifndef _missio_json_write_hpp
#define _missio_json_write_hpp

#pragma once

#include "export.hpp"

#include <string>
#include <iosfwd>


namespace missio::json
{

class value;

MISSIO_JSON_EXPORT void write(std::string& str, value const& value);
MISSIO_JSON_EXPORT void write(std::ostream& os, value const& value);

} // namespace missio::json

#endif // _missio_json_write_hpp
