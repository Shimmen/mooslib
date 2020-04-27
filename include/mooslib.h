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

#include <cmath> // for basic math functions
#include <cstdint> // for integer definitions
#include <fstream> // for readEntireFile etc.
#include <sstream> // for readEntireFile etc.
#include <string> // for readEntireFile etc.

namespace moos {

// Options

#ifdef MOOSLIB_USE_DOUBLE_BY_DEFAULT
using Float = double;
#else
using Float = float;
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

// TODO: Maybe define a custom assert!

// Basic file IO

std::string readEntireFile(const std::string& path)
{
    std::ifstream ifs(path);
    std::stringstream buffer;
    buffer << ifs.rdbuf();
    return buffer.str();
}

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

#define ENABLE_IF_FLOATING_POINT(T) typename = typename std::enable_if<IsFloatingPoint<T>::value>::type
#define ENABLE_IF_ARITHMETIC(T) typename = typename std::enable_if<std::is_arithmetic<T>::value>::type

// Math constants & basic math functions

constexpr Float E = 2.718281828459;
constexpr Float PI = 3.141592653590;
constexpr Float TWO_PI = 2.0 * PI;

template<typename T>
constexpr T square(T x)
{
    return x * x;
}

template<typename T, ENABLE_IF_FLOATING_POINT(T)>
constexpr T lerp(T a, T b, T x)
{
    // TODO: Implement for vec2 & vec3 too!
    return (1.0 - x) * a + x * b;
}

template<typename T>
constexpr T clamp(T x, T min, T max)
{
    // TODO: Implement for vec2 & vec3 too!
    return std::max(min, std::min(x, max));
}

constexpr Float radians(Float degrees)
{
    return degrees / 180.0 * PI;
}

constexpr Float degrees(Float radians)
{
    return radians / PI * 180.0;
}

// Easing functions
// TODO: Implement some of the important ones at least!

// Vectors

template<typename T, typename _ = void>
struct tvec2 {
};

template<typename T>
T* value_ptr(tvec2<T>& v)
{
    return static_cast<T*>(&v);
}

template<typename T>
const T* value_ptr(const tvec2<T>& v)
{
    return static_cast<const T*>(&v);
}

template<typename T>
struct tvec2<T, typename std::enable_if<std::is_arithmetic<T>::value>::type> {
    T x, y;

    constexpr tvec2(T x, T y)
        : x(x)
        , y(y)
    {
    }

    explicit constexpr tvec2(T e = 0)
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

    constexpr tvec2<T> operator*(const T& f) const { return { x * f, y * f }; }
    constexpr tvec2<T>& operator*=(const T& f)
    {
        x *= f;
        y *= f;
        return *this;
    }

    constexpr tvec2<T> operator/(const T& f) const { return { x / f, y / f }; }
    constexpr tvec2<T>& operator/=(const T& f)
    {
        x /= f;
        y /= f;
        return *this;
    }
};

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
constexpr tvec2<T> normalize(const tvec2<T>& v)
{
    return v / length(v);
}

template<>
struct tvec2<bool> {
    bool x, y;

    constexpr tvec2(bool x, bool y)
        : x(x)
        , y(y)
    {
    }

    explicit constexpr tvec2(bool e = false)
        : tvec2(e, e)
    {
    }

    constexpr tvec2<bool> operator~() const { return { !x, !y }; }
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
    return static_cast<T*>(&v);
}

template<typename T>
const T* value_ptr(const tvec3<T>& v)
{
    return static_cast<const T*>(&v);
}

template<typename T>
struct tvec3<T, typename std::enable_if<std::is_arithmetic<T>::value>::type> {
    T x, y, z;

    constexpr tvec3(T x, T y, T z)
        : x(x)
        , y(y)
        , z(z)
    {
    }

    explicit constexpr tvec3(T e = 0)
        : tvec3(e, e, e)
    {
    }

    constexpr tvec3<T> operator+() const { return *this; }
    constexpr tvec3<T> operator-() const { return { -x, -y, -z }; }

    constexpr tvec3<T> operator+(const tvec3<T>& v) const { return { x + v.x, y + v.y, z + v.z }; }
    constexpr tvec3<T> operator-(const tvec3<T>& v) const { return { x - v.x, y - v.y, z - v.z }; }

    constexpr tvec3<T> operator*(const T& f) const { return { x * f, y * f, z * f }; }
    constexpr tvec3<T>& operator*=(const T& f)
    {
        x *= f;
        y *= f;
        z *= f;
        return *this;
    }

    constexpr tvec3<T> operator/(const T& f) const { return { x / f, y / f, z / f }; }
    constexpr tvec3<T>& operator/=(const T& f)
    {
        x /= f;
        y /= f;
        z /= f;
        return *this;
    }
};

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
constexpr tvec3<T> normalize(const tvec3<T>& v)
{
    return v / length(v);
}

template<>
struct tvec3<bool> {
    bool x, y, z;

    constexpr tvec3(bool x, bool y, bool z)
        : x(x)
        , y(y)
        , z(z)
    {
    }

    explicit constexpr tvec3(bool e = false)
        : tvec3(e, e, e)
    {
    }

    constexpr tvec3<bool> operator~() const { return { !x, !y, !z }; }
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
    return static_cast<T*>(&v);
}

template<typename T>
const T* value_ptr(const tvec4<T>& v)
{
    return static_cast<const T*>(&v);
}

template<typename T>
struct tvec4<T, typename std::enable_if<std::is_arithmetic<T>::value>::type> {
    T x, y, z, w;

    constexpr tvec4(T x, T y, T z, T w)
        : x(x)
        , y(y)
        , z(z)
        , w(w)
    {
    }

    explicit constexpr tvec4(T e = 0)
        : tvec4(e, e, e, e)
    {
    }

    explicit constexpr tvec4(tvec3<T> v, T e = 1)
        : tvec4(v.x, v.y, v.z, e)
    {
    }
};

using vec4 = tvec4<Float>;
using fvec4 = tvec4<f32>;
using dvec4 = tvec4<f64>;
using uvec4 = tvec4<u32>;
using ivec4 = tvec4<i32>;

// Vector math constants

constexpr vec3 globalX = vec3(1, 0, 0);
constexpr vec3 globalY = vec3(0, 1, 0);
constexpr vec3 globalZ = vec3(0, 0, 1);

// (NOTE: Using a y-up right-handed coordinate system)
constexpr vec3 globalRight = +globalX;
constexpr vec3 globalUp = +globalY;
constexpr vec3 globalForward = -globalZ;

// Quaternion

template<typename T, typename _ = void>
struct tquat {
};

template<typename T>
struct tquat<T, typename std::enable_if<std::is_arithmetic<T>::value>::type> {
    tvec3<T> xyz;
    T w;

    // TODO: Add multiplication & vector transformations & to_matrix(quat)
};

using quat = tquat<Float>;
using fquat = tquat<f32>;
using dquat = tquat<f64>;

// Matrices
// TODO: Add multiplication & vector transformations

template<typename T>
struct tmat3 {
    tvec3<T> x, y, z;
};

using mat3 = tmat3<Float>;
using fmat3 = tmat3<f32>;
using dmat3 = tmat3<f64>;

template<typename T>
struct tmat4 {
    tvec4<T> x, y, z, w;

    // TODO: Hmm, should we avoid member functions to keep the API consistent maybe?
    tvec3<T> translation() const { return vec3(w); }
    void setTranslation(tvec3<T> t) { w = vec4(t); }
};

using mat4 = tmat4<Float>;
using fmat4 = tmat4<f32>;
using dmat4 = tmat4<f64>;

// Transformations & projections (only right-handed operations though)
// TODO: translate, scale, rotate (axis-angle & quat), perspective (with and without far plane), orthographic

// Axis-aligned bounding box (AABB)
// TODO: is-point-inside test, etc.
struct aabb3 {
    vec3 min;
    vec3 max;
};

// Ray tracing
// TODO: add basic intersection tests like ray-plane & ray-triangle & ray-aabb

struct Ray {
    vec3 origin;
    vec3 direction;
};

struct Plane {
    vec3 normal;
    Float distance;
};

struct Triangle {
    vec3 v0, v1, v2;
};

// Coordinate system conversions
// TODO: vector to spherical, vice versa

// Color utilities
// TODO: HSV to RGB, sRGB linear to display conversion etc.

// Random number generation
// TODO: randomInt, randomFloat, etc. make an object containing a random generator from std::random and expose some utility functions

} // namespace moos
