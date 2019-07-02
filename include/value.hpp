#ifndef _missio_json_value_hpp
#define _missio_json_value_hpp

#pragma once

#include "types.hpp"
#include "traits.hpp"
#include "export.hpp"

#include <cstddef>
#include <iosfwd>


namespace missio::json
{

class MISSIO_JSON_EXPORT value final
{
public:
    value() = default;

    value(boolean val) noexcept;
    value(integer val) noexcept;

    value(decimal val);
    value(string&& val);

    value(array&& val) noexcept;
    value(object&& val) noexcept;

    value(char const* val);
    value(string const& val);

    value(array const& val);
    value(object const& val);

    value(json::type type) noexcept;

    value(std::initializer_list<object::value_type> init);

    template <typename T>
    value(convertible_to_json<T> const& val);

    template <typename T>
    operator convertible_from_json<T>() const;

    template <typename T>
    convertible_from_json<T> as() const;

    value(value&& other) noexcept;
    value& operator=(value&& other) noexcept;

    value(value const& other);
    value& operator=(value const& other);

    ~value() noexcept;

    void swap(value& other) noexcept;

    void reset() noexcept;

    template <typename T>
    bool is() const noexcept;

    template <typename T>
    decltype(auto) get() const;

    operator boolean() const;
    operator integer() const;
    operator decimal() const;

    operator array&();
    operator object&();

    operator string const&() const;
    operator array const&() const;
    operator object const&() const;

    json::type type() const noexcept;

    bool is_null() const noexcept;
    bool is_true() const noexcept;
    bool is_false() const noexcept;
    bool is_boolean() const noexcept;
    bool is_integer() const noexcept;
    bool is_decimal() const noexcept;
    bool is_number() const noexcept;
    bool is_string() const noexcept;
    bool is_array() const noexcept;
    bool is_object() const noexcept;

    boolean get_boolean() const;
    integer get_integer() const;
    decimal get_decimal() const;

    boolean as_boolean() const;
    integer as_integer() const;
    decimal as_decimal() const;
    string as_string() const;

    array& as_array();
    object& as_object();

    array& get_array();
    object& get_object();

    string const& get_string() const;
    array const& get_array() const;
    object const& get_object() const;

    array& convert_to_array() noexcept;
    object& convert_to_object() noexcept;

    std::string to_string() const;

    // array-like interface

    void erase(std::size_t index);

    void push_back(value&& val);
    void push_back(value const& val);

    value& at(std::size_t index);
    value const& at(std::size_t index) const;

    value& operator[](std::size_t index);
    value const& operator[](std::size_t index) const;

    // object-like interface

    void erase(string const& key);

    template <typename T>
    void insert(string&& key, T&& val);

    template <typename T>
    void insert(string const& key, T&& val);

    bool contains(string const& key) const;

    value& at(string const& key);
    value const& at(string const& key) const;

    template <typename T>
    value& operator[](object_key<T>&& key);

    template <typename T>
    value const& operator[](object_key<T> const& key) const;

public:
    friend bool operator<(value const& lhs, value const& rhs) noexcept;
    friend bool operator==(value const& lhs, value const& rhs) noexcept;

private:
    impl::storage storage_{};
};

MISSIO_JSON_EXPORT bool operator<(value const& lhs, value const& rhs) noexcept;
MISSIO_JSON_EXPORT bool operator==(value const& lhs, value const& rhs) noexcept;

// clang-format off
inline bool operator!=(value const& lhs, value const& rhs) noexcept { return !operator==(lhs, rhs); }
inline bool operator<=(value const& lhs, value const& rhs) noexcept { return !operator<(rhs, lhs); }
inline bool operator>=(value const& lhs, value const& rhs) noexcept { return !operator<(lhs, rhs); }
inline bool operator>(value const& lhs, value const& rhs) noexcept { return operator<(rhs, lhs); }
// clang-format on

MISSIO_JSON_EXPORT std::ostream& operator<<(std::ostream& os, value const& val);
MISSIO_JSON_EXPORT std::istream& operator>>(std::istream& is, value& val);

} // namespace missio::json

#include "value.inl"
#include "traits.inl"

#endif // _missio_json_value_hpp
