
template <typename T>
struct missio::json::convert<T, std::enable_if_t<std::is_enum_v<T>>>
{
    static value to_json(T val)
    {
        return static_cast<integer>(val);
    }

    static T from_json(value const& val)
    {
        return static_cast<T>(val.get_integer());
    }
};

template <typename T>
struct missio::json::convert<T, std::enable_if_t<std::is_integral_v<T>>>
{
    static value to_json(T val)
    {
        return static_cast<integer>(val);
    }

    static T from_json(value const& val)
    {
        return static_cast<T>(val.as_integer());
    }
};

template <typename T>
struct missio::json::convert<T, std::enable_if_t<std::is_floating_point_v<T>>>
{
    static value to_json(T val)
    {
        return static_cast<decimal>(val);
    }

    static T from_json(value const& val)
    {
        return static_cast<T>(val.as_decimal());
    }
};

template <typename T>
struct missio::json::convert<T, std::enable_if_t<missio::json::traits::is_bounded_array_v<T>>>
{
    static value to_json(T const& val)
    {
        return array(std::begin(val), std::end(val));
    }
};

template <typename T>
struct missio::json::convert<T, std::enable_if_t<missio::json::traits::is_sequence_container_v<T>>>
{
    static value to_json(T const& val)
    {
        return array(val.begin(), val.end());
    }

    static T from_json(value const& val)
    {
        auto const& array = val.get_array();
        return T(array.begin(), array.end());
    }
};

template <typename T>
struct missio::json::convert<T, std::enable_if_t<missio::json::traits::is_associative_container_v<T>>>
{
    static value to_json(T const& val)
    {
        return object(val.begin(), val.end());
    }

    static T from_json(value const& val)
    {
        auto const& object = val.get_object();
        return T(object.begin(), object.end());
    }
};
