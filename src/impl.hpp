#ifndef _missio_json_impl_hpp
#define _missio_json_impl_hpp

#pragma once

#include "error.hpp"
#include "utils.hpp"

#include <type_traits>
#include <system_error>
#include <algorithm>
#include <memory>
#include <cmath>
#include <utility>
#include <array>
#include <new>


namespace missio::json::impl
{

template <type>
struct type_of { using type = void; };

template <type Type>
using type_of_t = typename type_of<Type>::type;

template <> struct type_of<type::integer> { using type = integer; };
template <> struct type_of<type::decimal> { using type = decimal; };
template <> struct type_of<type::string>  { using type = string;  };
template <> struct type_of<type::array>   { using type = array;   };
template <> struct type_of<type::object>  { using type = object;  };


template <type Type>
struct is_stateful : std::negation<std::is_void<type_of_t<Type>>> {};

template <type Type>
inline constexpr bool is_stateful_v = is_stateful<Type>::value;


template <type Type>
decltype(auto) get(storage& s) noexcept
{
    return reinterpret_cast<type_of_t<Type>&>(s.data);
}

template <type Type>
decltype(auto) get(storage const& s) noexcept
{
    return reinterpret_cast<type_of_t<Type> const&>(s.data);
}


template <type Type>
void destroy(storage& s) noexcept
{
    if constexpr(is_stateful_v<Type>)
    {
        std::destroy_at(std::addressof(get<Type>(s)));
    }

    s.type = type::null;
}

template <type Type, typename... Args>
void construct(storage& s, Args&&... args)
{
    if constexpr(is_stateful_v<Type>)
    {
        ::new (&s.data) type_of_t<Type>(std::forward<Args>(args)...);
    }

    s.type = Type;
}

template <type Type>
void default_construct(storage& s) noexcept
{
    if constexpr(is_stateful_v<Type>)
    {
        construct<Type>(s);
    }
    else
    {
        s.type = Type;
    }
}

template <type Type>
void move_construct(storage& dst, storage& src) noexcept
{
    if constexpr(is_stateful_v<Type>)
    {
        construct<Type>(dst, std::move(get<Type>(src)));
    }
    else
    {
        dst.type = src.type;
    }

    destroy<Type>(src);
}

template <type Type>
void copy_construct(storage& dst, storage const& src)
{
    if constexpr(is_stateful_v<Type>)
    {
        construct<Type>(dst, get<Type>(src));
    }
    else
    {
        dst.type = src.type;
    }
}

template <type Type>
void move_assign([[maybe_unused]] storage& dst, storage& src) noexcept
{
    if constexpr(is_stateful_v<Type>)
    {
        get<Type>(dst) = std::move(get<Type>(src));
    }

    destroy<Type>(src);
}

template <type Type>
void copy_assign([[maybe_unused]] storage& dst, [[maybe_unused]] storage const& src)
{
    if constexpr(is_stateful_v<Type>)
    {
        get<Type>(dst) = get<Type>(src);
    }
}

template <type Type>
void swap([[maybe_unused]] storage& lhs, [[maybe_unused]] storage& rhs) noexcept
{
    if constexpr(is_stateful_v<Type>)
    {
        std::swap(get<Type>(lhs), get<Type>(rhs));
    }
}

template <type Type>
bool less([[maybe_unused]] storage const& lhs, [[maybe_unused]] storage const& rhs) noexcept
{
    if constexpr(is_stateful_v<Type>)
    {
        return get<Type>(lhs) < get<Type>(rhs);
    }
    else
    {
        return false;
    }
}

template <type Type>
bool equal([[maybe_unused]] storage const& lhs, [[maybe_unused]] storage const& rhs) noexcept
{
    if constexpr(is_stateful_v<Type>)
    {
        return get<Type>(lhs) == get<Type>(rhs);
    }
    else
    {
        return true;
    }
}


inline auto index_of(std::initializer_list<type> types, type type) noexcept
{
    return std::distance(types.begin(), std::find(types.begin(), types.end(), type));
}


template <type... Types>
struct basic_vtable
{
    static constexpr auto types = {Types...};

    static void destroy(storage& s) noexcept
    {
        static constexpr std::array vtable
        {
            &impl::destroy<Types>...
        };

        vtable[index_of(types, s.type)](s);
    }

    static void default_construct(type type, storage& s) noexcept
    {
        static constexpr std::array vtable
        {
            &impl::default_construct<Types>...
        };

        vtable[index_of(types, type)](s);
    }

    static void move_construct(storage& dst, storage& src) noexcept
    {
        static constexpr std::array vtable
        {
            &impl::move_construct<Types>...
        };

        vtable[index_of(types, src.type)](dst, src);
    }

    static void copy_construct(storage& dst, storage const& src)
    {
        static constexpr std::array vtable
        {
            &impl::copy_construct<Types>...
        };

        vtable[index_of(types, src.type)](dst, src);
    }

    static void move_assign(storage& dst, storage& src) noexcept
    {
        if(src.type == dst.type)
        {
            static constexpr std::array vtable
            {
                &impl::move_assign<Types>...
            };

            vtable[index_of(types, src.type)](dst, src);
        }
        else
        {
            destroy(dst);
            move_construct(dst, src);
        }
    }

    static void copy_assign(storage& dst, storage const& src)
    {
        if(src.type == dst.type)
        {
            static constexpr std::array vtable
            {
                &impl::copy_assign<Types>...
            };

            vtable[index_of(types, src.type)](dst, src);
        }
        else
        {
            destroy(dst);
            copy_construct(dst, src);
        }
    }

    static void swap(storage& lhs, storage& rhs) noexcept
    {
        if(lhs.type == rhs.type)
        {
            static constexpr std::array vtable
            {
                &impl::swap<Types>...
            };

            vtable[index_of(types, lhs.type)](lhs, rhs);
        }
        else if(lhs.type == type::null)
        {
            move_construct(lhs, rhs);
        }
        else if(rhs.type == type::null)
        {
            move_construct(rhs, lhs);
        }
        else
        {
            storage tmp;

            move_construct(tmp, rhs);
            move_assign(rhs, lhs);
            move_assign(lhs, tmp);
        }
    }

    static bool less(storage const& lhs, storage const& rhs) noexcept
    {
        if(lhs.type == rhs.type)
        {
            static constexpr std::array vtable
            {
                &impl::less<Types>...
            };

            vtable[index_of(types, lhs.type)](lhs, rhs);
        }

        return lhs.type < rhs.type;
    }

    static bool equal(storage const& lhs, storage const& rhs) noexcept
    {
        if(lhs.type == rhs.type)
        {
            static constexpr std::array vtable
            {
                &impl::equal<Types>...
            };

            vtable[index_of(types, lhs.type)](lhs, rhs);
        }

        return false;
    }
};

using vtable = basic_vtable
<
    type::null,
    type::true_,
    type::false_,
    type::integer,
    type::decimal,
    type::string,
    type::array,
    type::object
>;


static boolean as_boolean(storage const& s)
{
    switch(s.type)
    {
        case type::true_:
            return true;

        case type::false_:
            return false;

        case type::integer:
            return get<type::integer>(s);

        case type::decimal:
            return get<type::decimal>(s);

        default:
            throw std::system_error{errc::bad_boolean_cast};
    }
}

static integer as_integer(storage const& s)
{
    switch(s.type)
    {
        case type::true_:
            return integer{1};

        case type::false_:
            return integer{0};

        case type::integer:
            return get<type::integer>(s);

        case type::decimal:
            return std::llround(get<type::decimal>(s));

        default:
            throw std::system_error{errc::bad_number_cast};
    }
}

static decimal as_decimal(storage const& s)
{
    switch(s.type)
    {
        case type::true_:
            return decimal{1};

        case type::false_:
            return decimal{0};

        case type::integer:
            return decimal(get<type::integer>(s));

        case type::decimal:
            return get<type::decimal>(s);

        default:
            throw std::system_error{errc::bad_number_cast};
    }
}

static string as_string(storage const& s)
{
    switch(s.type)
    {
        case type::null:
            return string{"null"};

        case type::true_:
            return string{"true"};

        case type::false_:
            return string{"false"};

        case type::integer:
            return to_string(get<type::integer>(s));

        case type::decimal:
            return to_string(get<type::decimal>(s));

        case type::string:
            return get<type::string>(s);

        default:
            throw std::system_error{errc::bad_string_cast};
    }
}

} // namespace missio::json::impl

#endif // _missio_json_impl_hpp
