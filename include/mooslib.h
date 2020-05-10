/*
 * MIT License
 *
 * Copyright (c) 2020 Simon Moos
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#pragma once

#include <algorithm> // for std::min/max etc.
#include <cassert> // for the assert macro
#include <cmath> // for basic math functions
#include <cstdint> // for integer definitions
#include <limits> // for min/max/inf values
#include <random> // for random number generation
#include <type_traits> // for std::enable_if etc.

#ifndef MOOSLIB_NO_INTRINSICS
#include <intrin.h>
#endif

namespace moos {

// Options

// Redefine this to use any assert
#ifndef MOOSLIB_ASSERT
#define MOOSLIB_ASSERT(x) assert(x)
#endif

// Some types assume a default float precision or don't allow choosing precision per object,
// but instead globally. For these cases this option exist. By default a 32-bit float is used.
#ifdef MOOSLIB_USE_DOUBLE_BY_DEFAULT
using Float = double;
#else
using Float = float;
#endif

// When inverting a matrix we have to divide by the determinant, which may be zero. The
// redefine this macro to specify some custom behaviour to handle this divide by zero case.
#ifndef MOOSLIB_ON_BAD_DETERMINANT_IN_MATRIX_INVERSE
#define MOOSLIB_ON_BAD_DETERMINANT_IN_MATRIX_INVERSE() MOOSLIB_ASSERT(false)
#endif

// Explicit numeric types

using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using i64 = std::int64_t;

using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;

using f32 = float;
using f64 = double;

// Utilities & macros

template<typename T>
struct IsFloatingPoint {
    static constexpr bool value { false };
};

template<>
struct IsFloatingPoint<f32> {
    static constexpr bool value { true };
};

template<>
struct IsFloatingPoint<f64> {
    static constexpr bool value { true };
};

#define ENABLE_STRUCT_IF_ARITHMETIC(T) typename std::enable_if<std::is_arithmetic<T>::value>::type
#define ENABLE_STRUCT_IF_FLOATING_POINT(T) typename std::enable_if<IsFloatingPoint<T>::value>::type

#define ENABLE_IF_ARITHMETIC(T) typename = typename std::enable_if<std::is_arithmetic<T>::value>::type
#define ENABLE_IF_FLOATING_POINT(T) typename = typename std::enable_if<IsFloatingPoint<T>::value>::type
#define ENABLE_IF_INTEGRAL(T) typename = typename std::enable_if<std::is_integral<T>::value>::type

// Math constants & basic math functions

constexpr Float E = static_cast<Float>(2.718281828459);
constexpr Float PI = static_cast<Float>(3.141592653590);
constexpr Float HALF_PI = PI / static_cast<Float>(2.0);
constexpr Float TWO_PI = static_cast<Float>(2.0) * PI;

template<typename T, ENABLE_IF_ARITHMETIC(T)>
constexpr T square(T x)
{
    return x * x;
}

template<typename T, ENABLE_IF_FLOATING_POINT(T)>
constexpr T lerp(T a, T b, T x)
{
    return (static_cast<T>(1) - x) * a + x * b;
}

template<typename T, ENABLE_IF_ARITHMETIC(T)>
constexpr T clamp(T x, T min, T max)
{
    return std::max(min, std::min(x, max));
}

constexpr Float toRadians(Float degrees)
{
    return degrees / static_cast<Float>(180.0) * PI;
}

constexpr Float toDegrees(Float radians)
{
    return radians / PI * static_cast<Float>(180.0);
}

// Vectors

template<typename T, typename _ = void>
struct tvec2 {
};

template<typename T>
T* value_ptr(tvec2<T>& v)
{
    return reinterpret_cast<T*>(&v);
}

template<typename T>
const T* value_ptr(const tvec2<T>& v)
{
    return reinterpret_cast<const T*>(&v);
}

template<typename T>
struct tvec2<T, ENABLE_STRUCT_IF_ARITHMETIC(T)> {
    T x, y;

    constexpr tvec2(T x, T y) noexcept
        : x(x)
        , y(y)
    {
    }

    explicit constexpr tvec2(T e = static_cast<T>(0)) noexcept
        : tvec2(e, e)
    {
    }

    constexpr tvec2<T> operator+() const { return *this; }
    constexpr tvec2<T> operator-() const { return { -x, -y }; }

    constexpr tvec2<T> operator+(const tvec2<T>& v) const { return { x + v.x, y + v.y }; }
    constexpr tvec2<T>& operator+=(const tvec2<T>& v)
    {
        x += v.x;
        y += v.y;
        return *this;
    }

    constexpr tvec2<T> operator-(const tvec2<T>& v) const { return { x - v.x, y - v.y }; }
    constexpr tvec2<T>& operator-=(const tvec2<T>& v)
    {
        x -= v.x;
        y -= v.y;
        return *this;
    }

    constexpr tvec2<T> operator*(T f) const { return { x * f, y * f }; }
    constexpr tvec2<T>& operator*=(T f)
    {
        x *= f;
        y *= f;
        return *this;
    }

    constexpr tvec2<T> operator/(T f) const { return { x / f, y / f }; }
    constexpr tvec2<T>& operator/=(T f)
    {
        x /= f;
        y /= f;
        return *this;
    }
};

template<typename T, ENABLE_IF_ARITHMETIC(T)>
constexpr tvec2<T> operator*(T lhs, const tvec2<T>& rhs)
{
    return rhs * lhs;
}

template<typename T, ENABLE_IF_ARITHMETIC(T)>
constexpr T dot(const tvec2<T>& lhs, const tvec2<T>& rhs)
{
    return lhs.x * rhs.x + lhs.y * rhs.y;
}

template<typename T, ENABLE_IF_ARITHMETIC(T)>
constexpr T length2(const tvec2<T>& v)
{
    return dot(v, v);
}

template<typename T, ENABLE_IF_FLOATING_POINT(T)>
constexpr T length(const tvec2<T>& v)
{
    return std::sqrt(length2(v));
}

template<typename T, ENABLE_IF_FLOATING_POINT(T)>
constexpr T distance(const tvec2<T>& a, const tvec2<T>& b)
{
    return length(a - b);
}

template<typename T, ENABLE_IF_FLOATING_POINT(T)>
constexpr tvec2<T> normalize(const tvec2<T>& v)
{
    return v / length(v);
}

template<typename T, ENABLE_IF_ARITHMETIC(T)>
constexpr tvec2<T> min(const tvec2<T>& lhs, const tvec2<T>& rhs)
{
    return { std::min(lhs.x, rhs.x), std::min(lhs.y, rhs.y) };
}

template<typename T, ENABLE_IF_ARITHMETIC(T)>
constexpr tvec2<T> max(const tvec2<T>& lhs, const tvec2<T>& rhs)
{
    return { std::max(lhs.x, rhs.x), std::max(lhs.y, rhs.y) };
}

template<typename T, ENABLE_IF_FLOATING_POINT(T)>
constexpr tvec2<T> lerp(const tvec2<T>& a, const tvec2<T>& b, T x)
{
    return (static_cast<T>(1) - x) * a + x * b;
}

template<typename T, ENABLE_IF_ARITHMETIC(T)>
constexpr tvec2<T> clamp(const tvec2<T>& x, const tvec2<T>& minEdge, const tvec2<T>& maxEdge)
{
    return max(minEdge, min(x, maxEdge));
}

template<>
struct tvec2<bool> {
    bool x, y;

    constexpr tvec2(bool x, bool y) noexcept
        : x(x)
        , y(y)
    {
    }

    explicit constexpr tvec2(bool e = false) noexcept
        : tvec2(e, e)
    {
    }

    constexpr tvec2<bool> operator~() const { return { !x, !y }; }
    constexpr tvec2<bool> operator||(const tvec2<bool>& v) const { return { x || v.x, y || v.y }; }
    constexpr tvec2<bool> operator&&(const tvec2<bool>& v) const { return { x && v.x, y && v.y }; }
};

template<typename T, ENABLE_IF_ARITHMETIC(T)>
constexpr tvec2<bool> lessThan(const tvec2<T>& lhs, const tvec2<T>& rhs)
{
    return { lhs.x < rhs.x, lhs.y < rhs.y };
}

template<typename T, ENABLE_IF_ARITHMETIC(T)>
constexpr tvec2<bool> lessThanEqual(const tvec2<T>& lhs, const tvec2<T>& rhs)
{
    return { lhs.x <= rhs.x, lhs.y <= rhs.y };
}

template<typename T, ENABLE_IF_ARITHMETIC(T)>
constexpr tvec2<bool> greaterThan(const tvec2<T>& lhs, const tvec2<T>& rhs)
{
    return { lhs.x > rhs.x, lhs.y > rhs.y };
}

template<typename T, ENABLE_IF_ARITHMETIC(T)>
constexpr tvec2<bool> greaterThanEqual(const tvec2<T>& lhs, const tvec2<T>& rhs)
{
    return { lhs.x >= rhs.x, lhs.y >= rhs.y };
}

constexpr bool any(const tvec2<bool>& v)
{
    return v.x || v.y;
}

constexpr bool all(const tvec2<bool>& v)
{
    return v.x && v.y;
}

using vec2 = tvec2<Float>;
using fvec2 = tvec2<f32>;
using dvec2 = tvec2<f64>;
using uvec2 = tvec2<u32>;
using ivec2 = tvec2<i32>;
using bvec2 = tvec2<bool>;

template<typename T, typename _ = void>
struct tvec3 {
};

template<typename T>
T* value_ptr(tvec3<T>& v)
{
    return reinterpret_cast<T*>(&v);
}

template<typename T>
const T* value_ptr(const tvec3<T>& v)
{
    return reinterpret_cast<const T*>(&v);
}

template<typename T>
struct tvec3<T, ENABLE_STRUCT_IF_ARITHMETIC(T)> {
    T x, y, z;

    constexpr tvec3(T x, T y, T z) noexcept
        : x(x)
        , y(y)
        , z(z)
    {
    }

    explicit constexpr tvec3(T e = static_cast<T>(0)) noexcept
        : tvec3(e, e, e)
    {
    }

    constexpr tvec3<T> operator+() const { return *this; }
    constexpr tvec3<T> operator-() const { return { -x, -y, -z }; }

    constexpr tvec3<T> operator+(const tvec3<T>& v) const { return { x + v.x, y + v.y, z + v.z }; }
    constexpr tvec3<T> operator+(T t) const { return { x + t, y + t, z + t }; }

    constexpr tvec3<T> operator-(const tvec3<T>& v) const { return { x - v.x, y - v.y, z - v.z }; }
    constexpr tvec3<T> operator-(T t) const { return { x - t, y - t, z - t }; }

    constexpr tvec3<T> operator*(const tvec3<T>& v) const { return { x * v.x, y * v.y, z * v.z }; }
    constexpr tvec3<T> operator*=(const tvec3<T>& v)
    {
        x *= v.x;
        y *= v.y;
        z *= v.z;
        return *this;
    }

    constexpr tvec3<T> operator/(const tvec3<T>& v) const { return { x / v.x, y / v.y, z / v.z }; }
    constexpr tvec3<T> operator/=(const tvec3<T>& v)
    {
        x /= v.x;
        y /= v.y;
        z /= v.z;
        return *this;
    }

    constexpr tvec3<T> operator*(T f) const { return { x * f, y * f, z * f }; }
    constexpr tvec3<T>& operator*=(T f)
    {
        x *= f;
        y *= f;
        z *= f;
        return *this;
    }

    constexpr tvec3<T> operator/(T f) const { return { x / f, y / f, z / f }; }
    constexpr tvec3<T>& operator/=(T f)
    {
        x /= f;
        y /= f;
        z /= f;
        return *this;
    }
};

template<typename T, ENABLE_IF_ARITHMETIC(T)>
constexpr tvec3<T> operator*(T lhs, const tvec3<T>& rhs)
{
    return rhs * lhs;
}

template<typename T, ENABLE_IF_ARITHMETIC(T)>
constexpr tvec3<T> cross(const tvec3<T>& lhs, const tvec3<T>& rhs)
{
    return {
        lhs.y * rhs.z - lhs.z * rhs.y,
        lhs.z * rhs.x - lhs.x * rhs.z,
        lhs.x * rhs.y - lhs.y * rhs.x
    };
}

template<typename T, ENABLE_IF_ARITHMETIC(T)>
constexpr T dot(const tvec3<T>& lhs, const tvec3<T>& rhs)
{
    return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
}

template<typename T, ENABLE_IF_ARITHMETIC(T)>
constexpr T length2(const tvec3<T>& v)
{
    return dot(v, v);
}

template<typename T, ENABLE_IF_FLOATING_POINT(T)>
constexpr T length(const tvec3<T>& v)
{
    return std::sqrt(length2(v));
}

template<typename T, ENABLE_IF_FLOATING_POINT(T)>
constexpr T distance(const tvec3<T>& a, const tvec3<T>& b)
{
    return length(a - b);
}

template<typename T, ENABLE_IF_FLOATING_POINT(T)>
constexpr tvec3<T> normalize(const tvec3<T>& v)
{
    return v / length(v);
}

template<typename T, ENABLE_IF_ARITHMETIC(T)>
constexpr tvec3<T> min(const tvec3<T>& lhs, const tvec3<T>& rhs)
{
    return { std::min(lhs.x, rhs.x), std::min(lhs.y, rhs.y), std::min(lhs.z, rhs.z) };
}

template<typename T, ENABLE_IF_ARITHMETIC(T)>
constexpr tvec3<T> max(const tvec3<T>& lhs, const tvec3<T>& rhs)
{
    return { std::max(lhs.x, rhs.x), std::max(lhs.y, rhs.y), std::max(lhs.z, rhs.z) };
}

template<typename T, ENABLE_IF_FLOATING_POINT(T)>
constexpr tvec3<T> lerp(const tvec3<T>& a, const tvec3<T>& b, T x)
{
    return (static_cast<T>(1) - x) * a + x * b;
}

template<typename T, ENABLE_IF_ARITHMETIC(T)>
constexpr tvec3<T> clamp(const tvec3<T>& x, const tvec3<T>& minEdge, const tvec3<T>& maxEdge)
{
    return max(minEdge, min(x, maxEdge));
}

template<>
struct tvec3<bool> {
    bool x, y, z;

    constexpr tvec3(bool x, bool y, bool z) noexcept
        : x(x)
        , y(y)
        , z(z)
    {
    }

    explicit constexpr tvec3(bool e = false) noexcept
        : tvec3(e, e, e)
    {
    }

    constexpr tvec3<bool> operator~() const { return { !x, !y, !z }; }
    constexpr tvec3<bool> operator||(const tvec3<bool>& v) const { return { x || v.x, y || v.y, z || v.z }; }
    constexpr tvec3<bool> operator&&(const tvec3<bool>& v) const { return { x && v.x, y && v.y, z && v.z }; }
};

template<typename T, ENABLE_IF_ARITHMETIC(T)>
constexpr tvec3<bool> lessThan(const tvec3<T>& lhs, const tvec3<T>& rhs)
{
    return { lhs.x < rhs.x, lhs.y < rhs.y, lhs.z < rhs.z };
}

template<typename T, ENABLE_IF_ARITHMETIC(T)>
constexpr tvec3<bool> lessThanEqual(const tvec3<T>& lhs, const tvec3<T>& rhs)
{
    return { lhs.x <= rhs.x, lhs.y <= rhs.y, lhs.z <= rhs.z };
}

template<typename T, ENABLE_IF_ARITHMETIC(T)>
constexpr tvec3<bool> greaterThan(const tvec3<T>& lhs, const tvec3<T>& rhs)
{
    return { lhs.x > rhs.x, lhs.y > rhs.y, lhs.z > rhs.z };
}

template<typename T, ENABLE_IF_ARITHMETIC(T)>
constexpr tvec3<bool> greaterThanEqual(const tvec3<T>& lhs, const tvec3<T>& rhs)
{
    return { lhs.x >= rhs.x, lhs.y >= rhs.y, lhs.z >= rhs.z };
}

constexpr bool any(const tvec3<bool>& v)
{
    return v.x || v.y || v.z;
}

constexpr bool all(const tvec3<bool>& v)
{
    return v.x && v.y && v.z;
}

using vec3 = tvec3<Float>;
using fvec3 = tvec3<f32>;
using dvec3 = tvec3<f64>;
using uvec3 = tvec3<u32>;
using ivec3 = tvec3<i32>;
using bvec3 = tvec3<bool>;

template<typename T, typename _ = void>
struct tvec4 {
};

template<typename T>
T* value_ptr(tvec4<T>& v)
{
    return reinterpret_cast<T*>(&v);
}

template<typename T>
const T* value_ptr(const tvec4<T>& v)
{
    return reinterpret_cast<const T*>(&v);
}

template<typename T>
struct tvec4<T, ENABLE_STRUCT_IF_ARITHMETIC(T)> {
    T x, y, z, w;

    constexpr tvec4(T x, T y, T z, T w) noexcept
        : x(x)
        , y(y)
        , z(z)
        , w(w)
    {
    }

    explicit constexpr tvec4(T e = static_cast<T>(0)) noexcept
        : tvec4(e, e, e, e)
    {
    }

    constexpr tvec4(const tvec3<T>& v, T e) noexcept
        : tvec4(v.x, v.y, v.z, e)
    {
    }

    // (a rare member function to simulate swizzling)
    constexpr tvec3<T> xyz() const
    {
        return { x, y, z };
    }
};

template<typename T, ENABLE_IF_ARITHMETIC(T)>
constexpr T dot(const tvec4<T>& lhs, const tvec4<T>& rhs)
{
    return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z + lhs.w * rhs.w;
}

template<>
inline f32 dot(const tvec4<f32>& lhs, const tvec4<f32>& rhs)
{
#ifdef __SSE__
    auto* a = reinterpret_cast<const __m128*>(value_ptr(lhs));
    auto* b = reinterpret_cast<const __m128*>(value_ptr(rhs));
    __m128 prod = _mm_mul_ps(*a, *b);
    return prod[0] + prod[1] + prod[2] + prod[3];
#else
    return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z + lhs.w * rhs.w;
#endif
}

using vec4 = tvec4<Float>;
using fvec4 = tvec4<f32>;
using dvec4 = tvec4<f64>;
using uvec4 = tvec4<u32>;
using ivec4 = tvec4<i32>;

// Vector math constants

constexpr vec3 globalX = vec3(static_cast<Float>(1), static_cast<Float>(0), static_cast<Float>(0));
constexpr vec3 globalY = vec3(static_cast<Float>(0), static_cast<Float>(1), static_cast<Float>(0));
constexpr vec3 globalZ = vec3(static_cast<Float>(0), static_cast<Float>(0), static_cast<Float>(1));

// (NOTE: Using a y-up right-handed coordinate system)
constexpr vec3 globalRight = +globalX;
constexpr vec3 globalUp = +globalY;
constexpr vec3 globalForward = -globalZ;

// Quaternion

template<typename T, typename _ = void>
struct tquat {
};

template<typename T>
struct tquat<T, ENABLE_STRUCT_IF_FLOATING_POINT(T)> {
    tvec3<T> xyz;
    T w;

    constexpr tquat(tvec3<T> xyz, T w) noexcept
        : xyz(xyz)
        , w(w)
    {
    }

    constexpr tquat() noexcept
        : tquat({ static_cast<T>(0), static_cast<T>(0), static_cast<T>(0) }, static_cast<T>(1))
    {
    }

    constexpr tvec3<T> operator*(const tvec3<T>& v) const
    {
        // Method by Fabian 'ryg' Giessen who posted it on some now defunct forum. There is some info
        // at https://blog.molecular-matters.com/2013/05/24/a-faster-quaternion-vector-multiplication/.

        tvec3<T> t = static_cast<T>(2) * cross(xyz, v);
        tvec3<T> res = v + w * t + cross(xyz, t);

        return res;
    }
};

template<typename T, ENABLE_IF_FLOATING_POINT(T)>
constexpr tquat<T> axisAngle(const tvec3<T>& axis, T angle)
{
    T halfAngle = angle / static_cast<T>(2);
    tvec3<T> xyz = axis * std::sin(halfAngle);
    T w = std::cos(halfAngle);
    return tquat<T>(xyz, w);
}

template<typename T, ENABLE_IF_FLOATING_POINT(T)>
constexpr tvec3<T> rotateVector(const tquat<T>& q, const tvec3<T>& v)
{
    return q * v;
}

// TODO: Maybe add some functions for rotating a quaternion by some other quaternion?
//  For example if we could do something like q *= axisAngle(globalUp, dt) it would be
//  pretty neat, I think. Maybe it should't use operator overloads though. A function
//  like quat = rotateBy(quat, other) would be neat. Maybe another elusive member function?!

using quat = tquat<Float>;
using fquat = tquat<f32>;
using dquat = tquat<f64>;

// Matrices

template<typename T, typename _ = void>
struct tmat3 {
};

template<typename T>
T* value_ptr(tmat3<T>& v)
{
    return reinterpret_cast<T*>(&v);
}

template<typename T>
const T* value_ptr(const tmat3<T>& v)
{
    return reinterpret_cast<const T*>(&v);
}

template<typename T>
struct tmat3<T, ENABLE_STRUCT_IF_ARITHMETIC(T)> {
    tvec3<T> x, y, z;

    explicit tmat3(T d = static_cast<T>(1.0)) noexcept
        : x(d, static_cast<T>(0), static_cast<T>(0))
        , y(static_cast<T>(0), d, static_cast<T>(0))
        , z(static_cast<T>(0), static_cast<T>(0), d)
    {
    }

    tmat3(tvec3<T> x, tvec3<T> y, tvec3<T> z) noexcept
        : x(x)
        , y(y)
        , z(z)
    {
    }

    constexpr tmat3<T> operator*(const tmat3<T>& other) const
    {
        tmat3<T> trans = transpose(*this);
        return {
            { dot(trans.x, other.x), dot(trans.y, other.x), dot(trans.z, other.x) },
            { dot(trans.x, other.y), dot(trans.y, other.y), dot(trans.z, other.y) },
            { dot(trans.x, other.z), dot(trans.y, other.z), dot(trans.z, other.z) }
        };
    }

    constexpr tvec3<T> operator*(const tvec3<T>& v) const
    {
        // TODO: Maybe make a version which doesn't require transpose first!
        tmat3<T> trans = transpose(*this);
        return { dot(trans.x, v),
                 dot(trans.y, v),
                 dot(trans.z, v) };
    }

    constexpr tmat3<T>
    operator*(T f) const
    {
        return { f * x, f * y, f * z };
    }
};

template<typename T, ENABLE_IF_ARITHMETIC(T)>
constexpr tmat3<T> operator*(T lhs, const tmat3<T>& rhs)
{
    return rhs * lhs;
}

template<typename T, ENABLE_IF_ARITHMETIC(T)>
constexpr tmat3<T> transpose(const tmat3<T>& m)
{
    return {
        { m.x.x, m.y.x, m.z.x },
        { m.x.y, m.y.y, m.z.y },
        { m.x.z, m.y.z, m.z.z }
    };
}

template<typename T, ENABLE_IF_ARITHMETIC(T)>
constexpr T determinant(const tmat3<T>& m)
{
    return m.x.x * (m.y.y * m.z.z - m.y.z * m.z.y)
        - m.y.x * (m.x.y * m.z.z - m.z.y * m.x.z)
        + m.z.x * (m.x.y * m.y.z - m.y.y * m.x.z);
}

template<typename T, ENABLE_IF_FLOATING_POINT(T)>
constexpr tmat3<T> inverse(const tmat3<T>& m)
{
    // This function is a rewritten version of  https://stackoverflow.com/a/18504573

    T det = determinant(m);
    if (std::abs(det) < std::numeric_limits<T>::epsilon()) {
        MOOSLIB_ON_BAD_DETERMINANT_IN_MATRIX_INVERSE();
    }
    T invDet = static_cast<T>(1) / det;

    tmat3<T> res;

    res.x.x = (m.y.y * m.z.z - m.y.z * m.z.y) * invDet;
    res.y.x = (m.z.x * m.y.z - m.y.x * m.z.z) * invDet;
    res.z.x = (m.y.x * m.z.y - m.z.x * m.y.y) * invDet;

    res.x.y = (m.z.y * m.x.z - m.x.y * m.z.z) * invDet;
    res.y.y = (m.x.x * m.z.z - m.z.x * m.x.z) * invDet;
    res.z.y = (m.x.y * m.z.x - m.x.x * m.z.y) * invDet;

    res.x.z = (m.x.y * m.y.z - m.x.z * m.y.y) * invDet;
    res.y.z = (m.x.z * m.y.x - m.x.x * m.y.z) * invDet;
    res.z.z = (m.x.x * m.y.y - m.x.y * m.y.x) * invDet;

    return res;
}

using mat3 = tmat3<Float>;
using fmat3 = tmat3<f32>;
using dmat3 = tmat3<f64>;

template<typename T, typename _ = void>
struct tmat4 {
};

template<typename T>
T* value_ptr(tmat4<T>& v)
{
    return reinterpret_cast<T*>(&v);
}

template<typename T>
const T* value_ptr(const tmat4<T>& v)
{
    return reinterpret_cast<const T*>(&v);
}

template<typename T>
struct tmat4<T, ENABLE_STRUCT_IF_ARITHMETIC(T)> {
    tvec4<T> x, y, z, w;

    explicit tmat4(T d = static_cast<T>(1)) noexcept
        : x(d, static_cast<T>(0), static_cast<T>(0), static_cast<T>(0))
        , y(static_cast<T>(0), d, static_cast<T>(0), static_cast<T>(0))
        , z(static_cast<T>(0), static_cast<T>(0), d, static_cast<T>(0))
        , w(static_cast<T>(0), static_cast<T>(0), static_cast<T>(0), d)
    {
    }

    tmat4(tvec4<T> x, tvec4<T> y, tvec4<T> z, tvec4<T> w) noexcept
        : x(x)
        , y(y)
        , z(z)
        , w(w)
    {
    }

    constexpr tmat4<T> operator*(const tmat4<T>& other) const
    {
        // TODO: It might be possible to make an even faster SIMD specialization for f32 of this whole thing, not just the vec4 dot products!
        tmat4<T> trans = transpose(*this);
        return {
            { dot(trans.x, other.x), dot(trans.y, other.x), dot(trans.z, other.x), dot(trans.w, other.x) },
            { dot(trans.x, other.y), dot(trans.y, other.y), dot(trans.z, other.y), dot(trans.w, other.y) },
            { dot(trans.x, other.z), dot(trans.y, other.z), dot(trans.z, other.z), dot(trans.w, other.z) },
            { dot(trans.x, other.w), dot(trans.y, other.w), dot(trans.z, other.w), dot(trans.w, other.w) }
        };
    }

    constexpr tvec4<T> operator*(const tvec4<T>& v) const
    {
        // TODO: Maybe make a version which doesn't require transpose first!
        tmat3<T> trans = transpose(*this);
        return { dot(trans.x, v),
                 dot(trans.y, v),
                 dot(trans.z, v),
                 dot(trans.w, v) };
    }

    constexpr tmat4<T> operator*(T f) const
    {
        return { f * x, f * y, f * z, f * w };
    }
};

template<typename T, ENABLE_IF_ARITHMETIC(T)>
constexpr tmat4<T> operator*(T lhs, const tmat4<T>& rhs)
{
    return rhs * lhs;
}

template<typename T, ENABLE_IF_ARITHMETIC(T)>
constexpr tmat4<T> transpose(const tmat4<T>& m)
{
    return {
        { m.x.x, m.y.x, m.z.x, m.w.x },
        { m.x.y, m.y.y, m.z.y, m.w.y },
        { m.x.z, m.y.z, m.z.z, m.w.z },
        { m.x.w, m.y.w, m.z.w, m.w.w }
    };
}

template<typename T, ENABLE_IF_FLOATING_POINT(T)>
constexpr tmat4<T> inverse(const tmat4<T>& m)
{
    // This function is a rewritten version of mat4x4_invert https://github.com/datenwolf/linmath.h

    T s[6];
    T c[6];

    s[0] = m.x.x * m.y.y - m.y.x * m.x.y;
    s[1] = m.x.x * m.y.z - m.y.x * m.x.z;
    s[2] = m.x.x * m.y.w - m.y.x * m.x.w;
    s[3] = m.x.y * m.y.z - m.y.y * m.x.z;
    s[4] = m.x.y * m.y.w - m.y.y * m.x.w;
    s[5] = m.x.z * m.y.w - m.y.z * m.x.w;

    c[0] = m.z.x * m.w.y - m.w.x * m.z.y;
    c[1] = m.z.x * m.w.z - m.w.x * m.z.z;
    c[2] = m.z.x * m.w.w - m.w.x * m.z.w;
    c[3] = m.z.y * m.w.z - m.w.y * m.z.z;
    c[4] = m.z.y * m.w.w - m.w.y * m.z.w;
    c[5] = m.z.z * m.w.w - m.w.z * m.z.w;

    T det = s[0] * c[5] - s[1] * c[4] + s[2] * c[3] + s[3] * c[2] - s[4] * c[1] + s[5] * c[0];
    if (std::abs(det) < std::numeric_limits<T>::epsilon()) {
        MOOSLIB_ON_BAD_DETERMINANT_IN_MATRIX_INVERSE();
    }
    T invDet = static_cast<T>(1) / det;

    tmat4<T> res;

    res.x.x = (m.y.y * c[5] - m.y.z * c[4] + m.y.w * c[3]) * invDet;
    res.x.y = (-m.x.y * c[5] + m.x.z * c[4] - m.x.w * c[3]) * invDet;
    res.x.z = (m.w.y * s[5] - m.w.z * s[4] + m.w.w * s[3]) * invDet;
    res.x.w = (-m.z.y * s[5] + m.z.z * s[4] - m.z.w * s[3]) * invDet;

    res.y.x = (-m.y.x * c[5] + m.y.z * c[2] - m.y.w * c[1]) * invDet;
    res.y.y = (m.x.x * c[5] - m.x.z * c[2] + m.x.w * c[1]) * invDet;
    res.y.z = (-m.w.x * s[5] + m.w.z * s[2] - m.w.w * s[1]) * invDet;
    res.y.w = (m.z.x * s[5] - m.z.z * s[2] + m.z.w * s[1]) * invDet;

    res.z.x = (m.y.x * c[4] - m.y.y * c[2] + m.y.w * c[0]) * invDet;
    res.z.y = (-m.x.x * c[4] + m.x.y * c[2] - m.x.w * c[0]) * invDet;
    res.z.z = (m.w.x * s[4] - m.w.y * s[2] + m.w.w * s[0]) * invDet;
    res.z.w = (-m.z.x * s[4] + m.z.y * s[2] - m.z.w * s[0]) * invDet;

    res.w.x = (-m.y.x * c[3] + m.y.y * c[1] - m.y.z * c[0]) * invDet;
    res.w.y = (m.x.x * c[3] - m.x.y * c[1] + m.x.z * c[0]) * invDet;
    res.w.z = (-m.w.x * s[3] + m.w.y * s[1] - m.w.z * s[0]) * invDet;
    res.w.w = (m.z.x * s[3] - m.z.y * s[1] + m.z.z * s[0]) * invDet;

    return res;
}

template<typename T, ENABLE_IF_FLOATING_POINT(T)>
constexpr tmat4<T> toMatrix(const tquat<T>& q)
{
    // This function is a rewritten version of mat4x4_from_quat from https://github.com/datenwolf/linmath.h

    const T& a = q.w;
    const T& b = q.xyz.x;
    const T& c = q.xyz.y;
    const T& d = q.xyz.z;
    T a2 = square(a);
    T b2 = square(b);
    T c2 = square(c);
    T d2 = square(d);

    tmat4<T> res;

    res.x.x = a2 + b2 - c2 - d2;
    res.x.y = static_cast<T>(2) * (b * c + a * d);
    res.x.z = static_cast<T>(2) * (b * d - a * c);
    res.x.w = static_cast<T>(0);

    res.y.x = static_cast<T>(2) * (b * c - a * d);
    res.y.y = a2 - b2 + c2 - d2;
    res.y.z = static_cast<T>(2) * (c * d + a * b);
    res.y.w = static_cast<T>(0);

    res.z.x = static_cast<T>(2) * (b * d + a * c);
    res.z.y = static_cast<T>(2) * (c * d - a * b);
    res.z.z = a2 - b2 - c2 + d2;
    res.z.w = static_cast<T>(0);

    res.w.x = static_cast<T>(0);
    res.w.y = static_cast<T>(0);
    res.w.z = static_cast<T>(0);
    res.w.w = static_cast<T>(1);

    return res;
}

using mat4 = tmat4<Float>;
using fmat4 = tmat4<f32>;
using dmat4 = tmat4<f64>;

// Transformations & projections (only right-handed operations though)

template<typename T, ENABLE_IF_FLOATING_POINT(T)>
constexpr tmat4<T> scale(T s)
{
    tmat4<T> m(s);
    m.w.w = static_cast<T>(1);
    return m;
}

template<typename T, ENABLE_IF_FLOATING_POINT(T)>
constexpr tmat4<T> scale(const tvec3<T>& v)
{
    tmat4<T> m(static_cast<T>(1.0));
    m.x.x = v.x;
    m.y.y = v.y;
    m.z.z = v.z;
    return m;
}

template<typename T, ENABLE_IF_FLOATING_POINT(T)>
constexpr tmat4<T> translate(const tvec3<T>& v)
{
    tmat4<T> m(static_cast<T>(1.0));
    m.w = { v, static_cast<T>(1.0) };
    return m;
}

template<typename T, ENABLE_IF_FLOATING_POINT(T)>
constexpr tmat4<T> rotate(const tquat<T>& q)
{
    return toMatrix(q);
}

template<typename T, ENABLE_IF_FLOATING_POINT(T)>
constexpr tmat4<T> lookAt(const tvec3<T>& eye, const tvec3<T>& target, const tvec3<T>& tempUp = globalUp)
{
    tvec3<T> forward = -normalize(target - eye);
    tvec3<T> right = cross(tempUp, forward);
    tvec3<T> up = cross(forward, right);

    // TODO: Maybe make a version which doesn't require transpose?
    tmat4<T> mTrans({ right, static_cast<T>(0) }, { up, static_cast<T>(0) }, { forward, static_cast<T>(0) }, { eye, static_cast<T>(1) });
    tmat4<T> m = transpose(mTrans);

    return m;
}

template<typename T, ENABLE_IF_FLOATING_POINT(T)>
constexpr tmat4<T> perspectiveProjectionToVulkanClipSpace(T fovy, T aspectRatio, T zNear, T zFar)
{
    // Code rewritten from GLM: https://github.com/g-truc/glm/blob/master/glm/ext/matrix_clip_space.inl
    // Right-handed, depth in range [0, 1], and y-coordinates where the top is -1 and the bottom is +1.

    MOOSLIB_ASSERT(std::abs(aspectRatio - std::numeric_limits<T>::epsilon()) > static_cast<T>(0));
    MOOSLIB_ASSERT(std::abs(zFar - zNear) > std::numeric_limits<T>::epsilon());
    MOOSLIB_ASSERT(fovy > std::numeric_limits<T>::epsilon());

    T tanHalfFovy = std::tan(fovy / static_cast<T>(2));
    tmat4<T> m(static_cast<T>(0));

    m.x.x = static_cast<T>(1) / (aspectRatio * tanHalfFovy);
    m.y.y = -static_cast<T>(1) / (tanHalfFovy);
    m.z.z = zFar / (zNear - zFar);
    m.z.w = -static_cast<T>(1);
    m.w.z = -(zFar * zNear) / (zFar - zNear);

    return m;
}

template<typename T, ENABLE_IF_FLOATING_POINT(T)>
constexpr tmat4<T> perspectiveProjectionToOpenGLClipSpace(T fovy, T aspectRatio, T zNear, T zFar)
{
    // Code rewritten from GLM: https://github.com/g-truc/glm/blob/master/glm/ext/matrix_clip_space.inl
    // Right-handed, depth in range [-1, 1], and y-coordinates where the top is +1 and the bottom is -1.

    MOOSLIB_ASSERT(std::abs(aspectRatio - std::numeric_limits<T>::epsilon()) > static_cast<T>(0));
    MOOSLIB_ASSERT(std::abs(zFar - zNear) > std::numeric_limits<T>::epsilon());
    MOOSLIB_ASSERT(fovy > std::numeric_limits<T>::epsilon());

    T tanHalfFovy = std::tan(fovy / static_cast<T>(2));
    tmat4<T> m(static_cast<T>(0));

    m.x.x = static_cast<T>(1) / (aspectRatio * tanHalfFovy);
    m.y.y = static_cast<T>(1) / (tanHalfFovy);
    m.z.z = -(zFar + zNear) / (zFar - zNear);
    m.z.w = -static_cast<T>(1);
    m.w.z = -(static_cast<T>(2) * zFar * zNear) / (zFar - zNear);

    return m;
}

enum class OrthographicProjectionDepthMode {
    ZeroToOne,
    NegativeOneToOne
};

template<typename T, ENABLE_IF_FLOATING_POINT(T)>
constexpr tmat4<T> orthographicProjection(T left, T right, T bottom, T top, T zNear, T zFar, OrthographicProjectionDepthMode depthMode)
{
    // Code rewritten from GLM: https://github.com/g-truc/glm/blob/master/glm/ext/matrix_clip_space.inl
    // Right-handed with depth in range [0, 1].

    tmat4<T> m(static_cast<T>(1));

    m.x.x = static_cast<T>(2) / (right - left);
    m.y.y = static_cast<T>(2) / (top - bottom);
    m.w.x = -(right + left) / (right - left);
    m.w.y = -(top + bottom) / (top - bottom);

    switch (depthMode) {
    case OrthographicProjectionDepthMode::ZeroToOne:
        m.z.z = -static_cast<T>(1) / (zFar - zNear);
        m.w.z = -zNear / (zFar - zNear);
        break;
    case OrthographicProjectionDepthMode::NegativeOneToOne:
        m.z.z = -static_cast<T>(2) / (zFar - zNear);
        m.w.z = -(zFar + zNear) / (zFar - zNear);
        break;
    default:
        MOOSLIB_ASSERT(false);
    }

    return m;
}

template<typename T, ENABLE_IF_FLOATING_POINT(T)>
constexpr tmat4<T> orthographicProjectionToVulkanClipSpace(T size, T zNear, T zFar)
{
    T halfSize = size / static_cast<T>(2);
    return orthographicProjection(-halfSize, +halfSize, +halfSize, -halfSize, zNear, zFar, OrthographicProjectionDepthMode::ZeroToOne);
}

template<typename T, ENABLE_IF_FLOATING_POINT(T)>
constexpr tmat4<T> orthographicProjectionToOpenGLClipSpace(T size, T zNear, T zFar)
{
    T halfSize = size / static_cast<T>(2);
    return orthographicProjection(-halfSize, +halfSize, -halfSize, +halfSize, zNear, zFar, OrthographicProjectionDepthMode::NegativeOneToOne);
}

// Axis-aligned bounding box (AABB)

struct aabb3 {
    vec3 min;
    vec3 max;

    explicit aabb3(vec3 min = vec3(std::numeric_limits<Float>::infinity()), vec3 max = vec3(-std::numeric_limits<Float>::infinity()))
        : min(min)
        , max(max)
    {
    }

    aabb3& expandWithPoint(const vec3& point)
    {
        min = moos::min(point, min);
        max = moos::max(point, max);
        return *this;
    }

    bool contains(const vec3& point) const
    {
        return all(greaterThanEqual(point, min) && lessThanEqual(point, max));
    }
};

// Coordinate system conversions
// TODO: vector to spherical, vice versa

// Color utilities

Float luminance(const vec3& color)
{
    constexpr vec3 luma = vec3(0.2126, 0.7152, 0.0722);
    return dot(color, luma);
}

namespace ACES {

    // This code is modified from 'Baking Lab' by MJP and David Neubelt (licensed under the MIT license):
    // https://github.com/TheRealMJP/BakingLab/blob/master/BakingLab/ACES.hlsl, who state
    // "The code in this file was originally written by Stephen Hill (@self_shadow), who deserves all
    // credit for coming up with this fit and implementing it. Buy him a beer next time you see him. :)"

    // sRGB => XYZ => D65_2_D60 => AP1 => RRT_SAT
    const mat3 inputMatrix = mat3(
        { 0.59719, 0.07600, 0.02840 },
        { 0.35458, 0.90834, 0.13383 },
        { 0.04823, 0.01566, 0.83777 });

    // ODT_SAT => XYZ => D60_2_D65 => sRGB
    const mat3 outputMatrix = mat3(
        { 1.60475, -0.10208, -0.00327 },
        { -0.53108, 1.10813, -0.07276 },
        { -0.07367, -0.00605, 1.07602 });

    vec3 RRTAndODTFit(vec3 v)
    {
        vec3 a = v * (v + 0.0245786) - 0.000090537;
        vec3 b = v * (v * 0.983729 + 0.4329510) + 0.238081;
        return a / b;
    }

    vec3 tonemap(vec3 color)
    {
        color = inputMatrix * color;
        color = RRTAndODTFit(color);
        color = outputMatrix * color;
        color = clamp(color, vec3(0.0), vec3(1.0));
        return color;
    }
}

namespace colorspace {

    const mat3 linear_sRGB_to_XYZ = mat3(
        { 0.4123865632529917, 0.21263682167732384, 0.019330620152483987 },
        { 0.35759149092062537, 0.7151829818412507, 0.11919716364020845 },
        { 0.18045049120356368, 0.07218019648142547, 0.9503725870054354 });

    const mat3 XYZ_to_linear_sRGB = mat3(
        { 3.2410032329763587, -0.9692242522025166, 0.055639419851975444 },
        { -1.5373989694887855, 1.875929983695176, -0.20401120612390997 },
        { -0.4986158819963629, 0.041554226340084724, 1.0571489771875335 });

    namespace sRGB {
        Float gammaEncode(Float linear)
        {
            // (i.e. convert from linear sRGB to gamma-encoded sRGB)
            return (linear < 0.0031308)
                ? 12.92 * linear
                : 1.055 * std::pow(linear, 1.0 / 2.4) - 0.055;
        }

        Float gammaDecode(Float encoded)
        {
            // (i.e. convert from gamma-encoded sRGB to linear sRGB)
            return (encoded < 0.04045)
                ? encoded / 12.92
                : std::pow((encoded + 0.055) / 1.055, 2.4);
        }

        vec3 gammaEncode(const vec3& linear)
        {
            return { gammaEncode(linear.x), gammaEncode(linear.y), gammaEncode(linear.z) };
        }

        vec3 gammaDecode(const vec3& encoded)
        {
            return { gammaDecode(encoded.x), gammaDecode(encoded.y), gammaDecode(encoded.z) };
        }
    }

    namespace HSV {

        vec3 toRGB(const vec3& hsv)
        {
            // From https://en.wikipedia.org/wiki/HSL_and_HSV#HSV_to_RGB

            // make sure H is in range [0, 360] degrees
            Float h = hsv.x;
            h = std::fmod(h, 360.0);
            h += 360.0;
            h = std::fmod(h, 360.0);

            const Float& s = hsv.y;
            const Float& v = hsv.z;

            Float c = v * s;
            Float hPrim = h / 60.0;
            Float x = c * (1.0 - std::abs(std::fmod(hPrim, 2.0) - 1.0));
            Float m = v - c;

            if (hPrim <= 1.0)
                return { c + m, x + m, m };
            if (hPrim <= 2.0)
                return { x + m, c + m, m };
            if (hPrim <= 3.0)
                return { m, c + m, x + m };
            if (hPrim <= 4.0)
                return { m, x + m, c + m };
            if (hPrim <= 5.0)
                return { x + m, m, c + m };
            if (hPrim <= 6.0)
                return { c + m, m, x + m };

            MOOSLIB_ASSERT(false);
            return { 0.0, 0.0, 0.0 };
        }

        vec3 fromRGB(vec3 rgb)
        {
            // From https://en.wikipedia.org/wiki/HSL_and_HSV#From_RGB

            rgb = clamp(rgb, vec3(0.0), vec3(1.0));
            const Float& r = rgb.x;
            const Float& g = rgb.y;
            const Float& b = rgb.z;

            Float xMax = std::max(r, std::max(g, b));
            Float xMin = std::min(r, std::min(g, b));
            Float c = xMax - xMin;

            const Float& v = xMax;
            Float s = (v == 0.0)
                ? 0.0
                : c / v;

            Float h = 0.0;
            if (c == 0.0)
                h = 0.0;
            else if (v == r)
                h = 60.0 * (g - b) / c;
            else if (v == g)
                h = 60.0 * (2.0 + (b - r) / c);
            else if (v == b)
                h = 60.0 * (4.0 + (r - g) / c);
            else
                MOOSLIB_ASSERT(false);

            return { h, s, v };
        }
    }
}

// Random number generation

class Random {
public:
    // Seeded by the system, similarly to time(NULL)
    explicit Random()
        : m_engine(std::random_device()())
    {
    }

    explicit Random(u64 seed)
        : m_engine(seed)
    {
    }

    static Random& instanceForThisThread()
    {
        static thread_local Random s_randomObjectForThread {};
        return s_randomObjectForThread;
    }

    template<typename T = Float, ENABLE_IF_FLOATING_POINT(T)>
    T randomFloatInRange(T minInclusive, T maxExclusive)
    {
        return std::uniform_real_distribution<T>(minInclusive, maxExclusive)(m_engine);
    }

    template<typename T = Float, ENABLE_IF_FLOATING_POINT(T)>
    T randomFloat()
    {
        return randomFloatInRange(0.0, 1.0);
    }

    template<typename T = i64, ENABLE_IF_INTEGRAL(T)>
    T randomIntInRange(T minInclusive, T maxInclusive)
    {
        return std::uniform_int_distribution<T>(minInclusive, maxInclusive)(m_engine);
    }

private:
    std::mt19937_64 m_engine;
};

} // namespace moos
