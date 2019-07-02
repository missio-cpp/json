#include "value.hpp"
#include "write.hpp"
#include "parse.hpp"
#include "error.hpp"
#include "impl.hpp"
#include "utf8.hpp"

#include <system_error>
#include <iostream>
#include <cmath>


missio::json::value::value(boolean val) noexcept
{
    if(val)
    {
        impl::default_construct<type::true_>(storage_);
    }
    else
    {
        impl::default_construct<type::false_>(storage_);
    }
}

missio::json::value::value(integer val) noexcept
{
    impl::construct<type::integer>(storage_, val);
}

missio::json::value::value(decimal val)
{
    if(!std::isfinite(val))
    {
        throw std::system_error{errc::bad_decimal_number};
    }

    impl::construct<type::decimal>(storage_, val);
}

missio::json::value::value(string&& val)
{
    if(!utf8::is_valid(val.begin(), val.end()))
    {
        throw std::system_error{errc::bad_utf8_sequence};
    }

    impl::construct<type::string>(storage_, std::move(val));
}

missio::json::value::value(array&& val) noexcept
{
    impl::construct<type::array>(storage_, std::move(val));
}

missio::json::value::value(object&& val) noexcept
{
    impl::construct<type::object>(storage_, std::move(val));
}

missio::json::value::value(char const* val) :
    value(string{val})
{}

missio::json::value::value(string const& val)
{
    if(!utf8::is_valid(val.begin(), val.end()))
    {
        throw std::system_error{errc::bad_utf8_sequence};
    }

    impl::construct<type::string>(storage_, val);
}

missio::json::value::value(array const& val)
{
    impl::construct<type::array>(storage_, val);
}

missio::json::value::value(object const& val)
{
    impl::construct<type::object>(storage_, val);
}

missio::json::value::value(json::type type) noexcept
{
    impl::vtable::default_construct(type, storage_);
}

missio::json::value::value(std::initializer_list<object::value_type> init) :
    value(object{init})
{}

missio::json::value::value(value&& other) noexcept
{
    impl::vtable::move_construct(storage_, other.storage_);
}

missio::json::value& missio::json::value::operator=(value&& other) noexcept
{
    impl::vtable::move_assign(storage_, other.storage_);

    return *this;
}

missio::json::value::value(value const& other)
{
    impl::vtable::copy_construct(storage_, other.storage_);
}

missio::json::value& missio::json::value::operator=(value const& other)
{
    if(this != &other)
    {
        impl::vtable::copy_assign(storage_, other.storage_);
    }

    return *this;
}

missio::json::value::~value() noexcept
{
    impl::vtable::destroy(storage_);
}

void missio::json::value::swap(value& other) noexcept
{
    if(this != &other)
    {
        impl::vtable::swap(storage_, other.storage_);
    }
}

void missio::json::value::reset() noexcept
{
    impl::vtable::destroy(storage_);
}

missio::json::value::operator missio::json::boolean() const
{
    return as_boolean();
}

missio::json::value::operator missio::json::integer() const
{
    return as_integer();
}

missio::json::value::operator missio::json::decimal() const
{
    return as_decimal();
}

missio::json::value::operator missio::json::array&()
{
    return as_array();
}

missio::json::value::operator missio::json::object&()
{
    return as_object();
}

missio::json::value::operator missio::json::string const&() const
{
    return get_string();
}

missio::json::value::operator missio::json::array const&() const
{
    return get_array();
}

missio::json::value::operator missio::json::object const&() const
{
    return get_object();
}

missio::json::type missio::json::value::type() const noexcept
{
    return storage_.type;
}

bool missio::json::value::is_null() const noexcept
{
    return type() == type::null;
}

bool missio::json::value::is_true() const noexcept
{
    return type() == type::true_;
}

bool missio::json::value::is_false() const noexcept
{
    return type() == type::false_;
}

bool missio::json::value::is_boolean() const noexcept
{
    return is_true() || is_false();
}

bool missio::json::value::is_integer() const noexcept
{
    return type() == type::integer;
}

bool missio::json::value::is_decimal() const noexcept
{
    return type() == type::decimal;
}

bool missio::json::value::is_number() const noexcept
{
    return is_integer() || is_decimal();
}

bool missio::json::value::is_string() const noexcept
{
    return type() == type::string;
}

bool missio::json::value::is_object() const noexcept
{
    return type() == type::object;
}

bool missio::json::value::is_array() const noexcept
{
    return type() == type::array;
}

missio::json::boolean missio::json::value::get_boolean() const
{
    if(is_true())
    {
        return true;
    }

    if(is_false())
    {
        return false;
    }

    throw std::system_error{errc::bad_boolean_cast};
}

missio::json::integer missio::json::value::get_integer() const
{
    if(is_integer())
    {
        return impl::get<type::integer>(storage_);
    }

    throw std::system_error{errc::bad_number_cast};
}

missio::json::decimal missio::json::value::get_decimal() const
{
    if(is_decimal())
    {
        return impl::get<type::decimal>(storage_);
    }

    throw std::system_error{errc::bad_number_cast};
}

missio::json::boolean missio::json::value::as_boolean() const
{
    return impl::as_boolean(storage_);
}

missio::json::integer missio::json::value::as_integer() const
{
    return impl::as_integer(storage_);
}

missio::json::decimal missio::json::value::as_decimal() const
{
    return impl::as_decimal(storage_);
}

missio::json::string missio::json::value::as_string() const
{
    return impl::as_string(storage_);
}

missio::json::array& missio::json::value::as_array()
{
    return is_null() ? convert_to_array() : get_array();
}

missio::json::object& missio::json::value::as_object()
{
    return is_null() ? convert_to_object() : get_object();
}

missio::json::array& missio::json::value::get_array()
{
    if(is_array())
    {
        return impl::get<type::array>(storage_);
    }

    throw std::system_error{errc::bad_array_cast};
}

missio::json::object& missio::json::value::get_object()
{
    if(is_object())
    {
        return impl::get<type::object>(storage_);
    }

    throw std::system_error{errc::bad_object_cast};
}

missio::json::string const& missio::json::value::get_string() const
{
    if(is_string())
    {
        return impl::get<type::string>(storage_);
    }

    throw std::system_error{errc::bad_string_cast};
}

missio::json::array const& missio::json::value::get_array() const
{
    if(is_array())
    {
        return impl::get<type::array>(storage_);
    }

    throw std::system_error{errc::bad_array_cast};
}

missio::json::object const& missio::json::value::get_object() const
{
    if(is_object())
    {
        return impl::get<type::object>(storage_);
    }

    throw std::system_error{errc::bad_object_cast};
}

missio::json::array& missio::json::value::convert_to_array() noexcept
{
    if(!is_array())
    {
        impl::vtable::destroy(storage_);
        impl::default_construct<type::array>(storage_);
    }

    return impl::get<type::array>(storage_);
}

missio::json::object& missio::json::value::convert_to_object() noexcept
{
    if(!is_object())
    {
        impl::vtable::destroy(storage_);
        impl::default_construct<type::object>(storage_);
    }

    return impl::get<type::object>(storage_);
}

std::string missio::json::value::to_string() const
{
    std::string result;

    write(result, *this);

    return result;
}

void missio::json::value::erase(std::size_t index)
{
    array& array = get_array();

    if(index >= array.size())
    {
        throw std::out_of_range{"invalid array index: " + std::to_string(index)};
    }

    array.erase(std::next(array.begin(), index));
}

void missio::json::value::push_back(value&& val)
{
    as_array().push_back(std::move(val));
}

void missio::json::value::push_back(value const& val)
{
    as_array().push_back(val);
}

missio::json::value& missio::json::value::at(std::size_t index)
{
    try
    {
        return get_array().at(index);
    }
    catch(std::out_of_range const&)
    {
        throw std::out_of_range{"invalid array index: " + std::to_string(index)};
    }
}

missio::json::value const& missio::json::value::at(std::size_t index) const
{
    try
    {
        return get_array().at(index);
    }
    catch(std::out_of_range const&)
    {
        throw std::out_of_range{"invalid array index: " + std::to_string(index)};
    }
}

missio::json::value& missio::json::value::operator[](std::size_t index)
{
    array& array = as_array();

    if(index >= array.size())
    {
        array.resize(index + 1);
    }

    return array[index];
}

missio::json::value const& missio::json::value::operator[](std::size_t index) const
{
    try
    {
        return get_array().at(index);
    }
    catch(std::out_of_range const&)
    {
        throw std::out_of_range{"invalid array index: " + std::to_string(index)};
    }
}

void missio::json::value::erase(string const& key)
{
    get_object().erase(key);
}

bool missio::json::value::contains(string const& key) const
{
    if(is_object())
    {
        object const& object = get_object();
        return object.find(key) != object.end();
    }

    return false;
}

missio::json::value& missio::json::value::at(string const& key)
{
    try
    {
        return get_object().at(key);
    }
    catch(std::out_of_range const&)
    {
        throw std::out_of_range{"invalid object key: " + key};
    }
}

missio::json::value const& missio::json::value::at(string const& key) const
{
    try
    {
        return get_object().at(key);
    }
    catch(std::out_of_range const&)
    {
        throw std::out_of_range{"invalid object key: " + key};
    }
}

bool missio::json::operator<(value const& lhs, value const& rhs) noexcept
{
    return impl::vtable::less(lhs.storage_, rhs.storage_);
}

bool missio::json::operator==(value const& lhs, value const& rhs) noexcept
{
    return impl::vtable::equal(lhs.storage_, rhs.storage_);
}

std::ostream& missio::json::operator<<(std::ostream& os, value const& val)
{
	std::ostream::sentry sentry{os};

    if(sentry)
    {
        write(os, val);
    }

    return os;
}

std::istream& missio::json::operator>>(std::istream& is, value& val)
{
    std::istream::sentry sentry{is};

    if(sentry)
    {
        val = parse(is);
    }

    return is;
}
