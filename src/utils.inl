
inline constexpr bool is_control(char c) noexcept
{
    return static_cast<unsigned>(c) <= 0x1F;
}

inline constexpr bool is_digit(char c) noexcept
{
    return c >= '0' && c <= '9';
}

inline constexpr bool is_xdigit(char c) noexcept
{
    return (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F') || is_digit(c);
}

inline constexpr bool is_space(char c) noexcept
{
    return c == '\t' || c == '\r' || c == '\n' || c == ' ';
}

inline constexpr bool is_delimiter(char c) noexcept
{
    return c == ',' || c == ':' || c == ']' || c == '}' || is_space(c);
}
