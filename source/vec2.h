#pragma once

#include <cmath>

#include <array>
#include <functional>
#include <iostream>
#include <type_traits>

#include <boost/container_hash/hash.hpp>

namespace dm {

template <typename T>
class Vec2
{
  public:
    using dimension_type = T;

    Vec2(T x, T y) : elements_{x, y} {}
    Vec2(std::initializer_list<T> list)
    {
        std::copy(list.begin(), list.end(), elements_.begin());
    }

    [[nodiscard]] static constexpr Vec2<T> unit_x() noexcept
    {
        return {1, 0};
    }

    [[nodiscard]] static constexpr Vec2<T> unit_y() noexcept
    {
        return {0, 1};
    }

    [[nodiscard]] std::array<T, 2> elements() const noexcept
    {
        return elements_;
    }

    [[nodiscard]] std::array<T, 2> elements() noexcept
    {
        return elements_;
    }

    [[nodiscard]] T x() const noexcept
    {
        return elements_[x_axis];
    }

    [[nodiscard]] T& x() noexcept
    {
        return elements_[x_axis];
    }

    [[nodiscard]] T y() const noexcept
    {
        return elements_[y_axis];
    }

    [[nodiscard]] T& y() noexcept
    {
        return elements_[y_axis];
    }

    [[nodiscard]] friend constexpr Vec2<T> operator-(const Vec2<T>& value) noexcept
    {
        return {-value.x(), -value.y()};
    }

    [[nodiscard]] friend constexpr Vec2<T> operator-(const Vec2<T>& lhs, const Vec2<T>& rhs) noexcept
    {
        return {lhs.x() - rhs.x(), lhs.y() - rhs.y()};
    }

    [[nodiscard]] friend constexpr Vec2<T> operator+(const Vec2<T>& lhs, const Vec2<T>& rhs) noexcept
    {
        return {lhs.x() + rhs.x(), lhs.y() + rhs.y()};
    }

    template <typename U>
    [[nodiscard]] friend constexpr Vec2<T> operator*(U n, const Vec2<T>& value) noexcept
    {
        return {n * value.x(), n * value.y()};
    }

    template <typename U>
    [[nodiscard]] friend constexpr Vec2<T> operator*(const Vec2<T>& value, U n) noexcept
    {
        return n * value;
    }

    template <typename U>
    [[nodiscard]] friend constexpr Vec2<T> operator/(const Vec2<T>& value, U n) noexcept
    {
        return {value.x() / n, value.y() / n};
    }

    constexpr Vec2<T>& operator-=(const Vec2<T>& other) noexcept
    {
        return *this = *this - other;
    }

    constexpr Vec2<T>& operator+=(const Vec2<T>& other) noexcept
    {
        return *this = *this + other;
    }

    constexpr Vec2<T>& operator*=(T n) noexcept
    {
        return *this = *this * n;
    }

    constexpr Vec2<T>& operator/=(T n) noexcept
    {
        return *this = *this / n;
    }

    [[nodiscard]] constexpr T magnitude_squared() const noexcept
    {
        return x() * x() + y() * y();
    }

    [[nodiscard]] constexpr T magnitude() const noexcept
    {
        if constexpr (std::is_integral_v<T>) {
            return std::llround(std::sqrt(magnitude_squared()));
        } else {
            return std::sqrt(magnitude_squared());
        }
    }

    constexpr Vec2<T>& normalize() noexcept
    {
        return *this /= magnitude();
    }

    [[nodiscard]] friend constexpr bool operator==(const Vec2<T>& lhs, const Vec2<T>& rhs) noexcept
    {
        return lhs.x() == rhs.x() && lhs.y() == rhs.y();
    }

    [[nodiscard]] friend constexpr bool operator!=(const Vec2<T>& lhs, const Vec2<T>& rhs) noexcept
    {
        return !(lhs == rhs);
    }

    [[nodiscard]] friend constexpr bool operator<(const Vec2<T>& lhs, const Vec2<T>& rhs) noexcept
    {
        return (lhs.x() != rhs.x()) ? lhs.x() < rhs.x() : lhs.y() < rhs.y();
    }

    [[nodiscard]] friend constexpr bool operator>(const Vec2<T>& lhs, const Vec2<T>& rhs) noexcept
    {
        return rhs < lhs;
    }

    [[nodiscard]] friend constexpr bool operator<=(const Vec2<T>& lhs, const Vec2<T>& rhs) noexcept
    {
        return !(lhs > rhs);
    }

    [[nodiscard]] friend constexpr bool operator>=(const Vec2<T>& lhs, const Vec2<T>& rhs) noexcept
    {
        return !(lhs < rhs);
    }

    [[nodiscard]] static constexpr T distance_squared(const Vec2<T>& lhs, const Vec2<T>& rhs) noexcept
    {
        Vec2<T> delta = abs_difference(lhs, rhs);
        return delta.x() * delta.x() + delta.y() * delta.y();
    }

    [[nodiscard]] static constexpr T distance(const Vec2<T>& lhs, const Vec2<T>& rhs) noexcept
    {
        return std::sqrt(distance_squared(lhs, rhs));
    }

    [[nodiscard]] static constexpr T chebyshev_distance(const Vec2<T>& lhs, const Vec2<T>& rhs) noexcept
    {
        Vec2<T> delta = abs_difference(lhs, rhs);
        return std::max(delta.x(), delta.y());
    }

    [[nodiscard]] static constexpr T manhattan_distance(const Vec2<T>& lhs, const Vec2<T>& rhs) noexcept
    {
        Vec2<T> delta = abs_difference(lhs, rhs);
        return delta.x() + delta.y();
    }

    friend std::ostream& operator<<(std::ostream& output, const Vec2<T>& value)
    {
        return output << "{" << value.x() << ", " << value.y() << "}";
    }

  private:
    static constexpr std::size_t x_axis = 0;
    static constexpr std::size_t y_axis = 1;

    std::array<T, 2> elements_;

    [[nodiscard]] static constexpr T abs_difference(T lhs, T rhs) noexcept
    {
        if constexpr (std::is_signed_v<T>) {
            return std::abs(lhs - rhs);
        } else {
            return std::max(lhs, rhs) - std::min(lhs, rhs);
        }
    }

    [[nodiscard]] static constexpr T abs_difference_x(const Vec2<T>& lhs, const Vec2<T>& rhs) noexcept
    {
        return abs_difference(lhs.x(), rhs.x());
    }

    [[nodiscard]] static constexpr T abs_difference_y(const Vec2<T>& lhs, const Vec2<T>& rhs) noexcept
    {
        return abs_difference(lhs.y(), rhs.y());
    }

    [[nodiscard]] static constexpr Vec2<T> abs_difference(const Vec2<T>& lhs, const Vec2<T>& rhs) noexcept
    {
        return {abs_difference(lhs.x(), rhs.x()), abs_difference(lhs.y(), rhs.y())};
    }
};

template <typename T>
class Coord : public Vec2<T>
{
  public:
    static constexpr Coord<T> unit_row()
    {
        return Vec2<T>::unit_x();
    }

    static constexpr Coord<T> unit_column()
    {
        return Vec2<T>::unit_y();
    }

    using Vec2<T>::Vec2;
    constexpr Coord(const Vec2<T>& other) : Vec2<T>{other} {}
    constexpr Coord(Vec2<T>&& other) : Vec2<T>{std::move(other)} {}
    constexpr Coord& operator=(const Vec2<T>& other)
    {
        return *this->Vec2<T>::operator=(other);
    }
    constexpr Coord& operator=(Vec2<T>&& other)
    {
        return *this->Vec2<T>::operator=(other);
    }

    T row() const
    {
        return Vec2<T>::x();
    }

    T& row()
    {
        return Vec2<T>::x();
    }

    T column() const
    {
        return Vec2<T>::y();
    }

    T& column()
    {
        return Vec2<T>::y();
    }
};

template <typename T>
class Size : public Vec2<T>
{
  public:
    using Vec2<T>::Vec2;

    constexpr Size(const Vec2<T>& other) : Vec2<T>{other} {}
    constexpr Size(Vec2<T>&& other) : Vec2<T>{std::move(other)} {}
    constexpr Size& operator=(const Vec2<T>& other)
    {
        return *this->Vec2<T>::operator=(other);
    }
    constexpr Size& operator=(Vec2<T>&& other)
    {
        return *this->Vec2<T>::operator=(other);
    }

    T height() const
    {
        return Vec2<T>::x();
    }

    T& height()
    {
        return Vec2<T>::x();
    }

    T width() const
    {
        return Vec2<T>::y();
    }

    T& width()
    {
        return Vec2<T>::y();
    }
};

template <typename Vec2Container>
[[nodiscard]] std::optional<typename Vec2Container::value_type::dimension_type> min_x(const Vec2Container& vec2s)
{
    if (vec2s.size() == 0) {
        return {};
    }
    return std::min_element(vec2s.cbegin(), vec2s.cend(), [](auto lhs, auto rhs) { return lhs.x() < rhs.x(); })->x();
}

template <typename Vec2Container>
[[nodiscard]] std::optional<typename Vec2Container::value_type::dimension_type> max_x(const Vec2Container& vec2s)
{
    if (vec2s.size() == 0) {
        return {};
    }
    return std::max_element(vec2s.cbegin(), vec2s.cend(), [](auto lhs, auto rhs) { return lhs.x() < rhs.x(); })->x();
}

template <typename Vec2Container>
[[nodiscard]] std::optional<typename Vec2Container::value_type::dimension_type> min_y(const Vec2Container& vec2s)
{
    if (vec2s.size() == 0) {
        return {};
    }
    return std::min_element(vec2s.cbegin(), vec2s.cend(), [](auto lhs, auto rhs) { return lhs.y() < rhs.y(); })->y();
}

template <typename Vec2Container>
[[nodiscard]] std::optional<typename Vec2Container::value_type::dimension_type> max_y(const Vec2Container& vec2s)
{
    if (vec2s.size() == 0) {
        return {};
    }
    return std::max_element(vec2s.cbegin(), vec2s.cend(), [](auto lhs, auto rhs) { return lhs.y() < rhs.y(); })->y();
}

template <typename Vec2Container>
[[nodiscard]] std::optional<typename Vec2Container::value_type> min_extent(const Vec2Container& vec2s)
{
    if (vec2s.size() == 0) {
        return {};
    }
    return {*min_x(vec2s), *min_y(vec2s)};
}

template <typename Vec2Container>
[[nodiscard]] std::optional<typename Vec2Container::value_type> max_extent(const Vec2Container& vec2s)
{
    if (vec2s.size() == 0) {
        return {};
    }
    return {*max_x(vec2s), *max_y(vec2s)};
}

template <typename Vec2Container>
[[nodiscard]] std::optional<std::pair<typename Vec2Container::value_type, typename Vec2Container::value_type>>
extents(const Vec2Container& vec2s)
{
    if (vec2s.size() == 0) {
        return {};
    }
    return {*min_extent(vec2s), *max_extent(vec2s)};
}

} // namespace dm

namespace std {
template <typename T>
struct hash<dm::Vec2<T>>
{
    size_t operator()(const dm::Vec2<T>& value) const
    {
        size_t result = 0;
        boost::hash_combine(result, value.x());
        boost::hash_combine(result, value.y());
        return result;
    }
};
} // namespace std
