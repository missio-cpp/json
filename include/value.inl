
template <typename T>
missio::json::value::value(convertible_to_json<T> const& val) :
    value(convert<T>::to_json(val))
{}

template <typename T>
missio::json::value::operator missio::json::convertible_from_json<T>() const
{
    return convert<T>::from_json(*this);
}

template <typename T>
missio::json::convertible_from_json<T> missio::json::value::as() const
{
    return convert<T>::from_json(*this);
}

template <>
inline bool missio::json::value::is<missio::json::boolean>() const noexcept
{
    return is_boolean();
}

template <>
inline bool missio::json::value::is<missio::json::integer>() const noexcept
{
    return is_integer();
}

template <>
inline bool missio::json::value::is<missio::json::decimal>() const noexcept
{
    return is_decimal();
}

template <>
inline bool missio::json::value::is<missio::json::string>() const noexcept
{
    return is_string();
}

template <>
inline bool missio::json::value::is<missio::json::array>() const noexcept
{
    return is_array();
}

template <>
inline bool missio::json::value::is<missio::json::object>() const noexcept
{
    return is_object();
}

template <>
inline decltype(auto) missio::json::value::get<missio::json::boolean>() const
{
    return get_boolean();
}

template <>
inline decltype(auto) missio::json::value::get<missio::json::integer>() const
{
    return get_integer();
}

template <>
inline decltype(auto) missio::json::value::get<missio::json::decimal>() const
{
    return get_decimal();
}

template <>
inline decltype(auto) missio::json::value::get<missio::json::string>() const
{
    return get_string();
}

template <>
inline decltype(auto) missio::json::value::get<missio::json::array>() const
{
    return get_array();
}

template <>
inline decltype(auto) missio::json::value::get<missio::json::object>() const
{
    return get_object();
}

template <typename T>
void missio::json::value::insert(string&& key, T&& val)
{
    as_object().insert_or_assign(std::move(key), std::forward<T>(val));
}

template <typename T>
void missio::json::value::insert(string const& key, T&& val)
{
    as_object().insert_or_assign(key, std::forward<T>(val));
}

template <typename T>
missio::json::value& missio::json::value::operator[](object_key<T>&& key)
{
    return as_object().operator[](std::forward<T>(key));
}

template <typename T>
missio::json::value const& missio::json::value::operator[](object_key<T> const& key) const
{
    return get_object().at(key);
}
