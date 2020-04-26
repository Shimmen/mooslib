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
#include <limits> // for numeric min/max values etc.
#include <sstream> // for readEntireFile etc.
#include <string> // for readEntireFile etc.

namespace moos {

// Numeric types

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

// Math constants & basic math functions

constexpr float E = 2.718281828459f;
constexpr float PI = 3.141592653590f;
constexpr float TWO_PI = 2.0f * PI;

template<typename T>
inline constexpr T square(T x)
{
    return x * x;
}

constexpr float radians(float degrees)
{
    return degrees / 180.0f * PI;
}

constexpr float degrees(float radians)
{
    return radians / PI * 180.0f;
}

// Utilities for vectors etc.

template<typename T>
struct VectorValue {
    static constexpr T null = 0;
    static constexpr T unit = 1;
};

template<>
struct VectorValue<bool> {
    static constexpr bool null = false;
    static constexpr bool unit = true;
};

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

// Vectors

template<typename T>
struct tvec2 {
    T x, y;

    constexpr tvec2(T x, T y)
        : x(x)
        , y(y)
    {
    }

    explicit constexpr tvec2(T e = VectorValue<T>::null)
        : tvec2(e, e)
    {
    }

    constexpr tvec2<T> operator+(const tvec2<T> v) const { return { x + v.x, y + v.y }; }
    constexpr tvec2<T> operator-(const tvec2<T> v) const { return { x - v.x, y - v.y }; }

    constexpr tvec2<T> operator*(T f) const { return { x * f, y * f }; }
    constexpr tvec2<T>& operator*=(T f)
    {
        *this = *this * f;
        return *this;
    }

    constexpr tvec2<T> operator/(T f) const { return { x / f, y / f }; }
    constexpr tvec2<T>& operator/=(T f)
    {
        *this = *this / f;
        return *this;
    }
};

template<typename T>
constexpr tvec2<T> operator+(const tvec2<T>& v) { return v; }

template<typename T>
constexpr tvec2<T> operator-(const tvec2<T>& v) { return { -v.x, -v.y }; }

template<typename T>
constexpr T dot(const tvec2<T>& lhs, const tvec2<T>& rhs)
{
    return lhs.x * rhs.x + lhs.y * rhs.y;
}

template<typename T>
constexpr T length2(const tvec2<T>& v)
{
    return dot(v, v);
}

template<typename T, typename = typename std::enable_if<IsFloatingPoint<T>::value>::type>
constexpr T length(const tvec2<T>& v)
{
    return std::sqrt(length2(v));
}

template<typename T, typename = typename std::enable_if<IsFloatingPoint<T>::value>::type>
constexpr tvec2<T> normalize(const tvec2<T>& v)
{
    return v / length(v);
}

using vec2 = tvec2<f32>;
using dvec2 = tvec2<f64>;
using uvec2 = tvec2<u32>;
using ivec2 = tvec2<i32>;
//using bvec2 = tvec2<bool>; // TODO: We will need a specialization for this (if we even need it)!

template<typename T>
struct tvec3 {
    T x, y, z;

    constexpr tvec3(T x, T y, T z)
        : x(x)
        , y(y)
        , z(z)
    {
    }

    explicit constexpr tvec3(T e = VectorValue<T>::null)
        : tvec3(e, e, e)
    {
    }

    constexpr tvec3<T> operator+(const tvec3<T> v) const { return { x + v.x, y + v.y, z + v.z }; }
    constexpr tvec3<T> operator-(const tvec3<T> v) const { return { x - v.x, y - v.y, z - v.z }; }

    constexpr tvec3<T> operator*(T f) const { return { x * f, y * f, z * f }; }
    constexpr tvec3<T>& operator*=(T f)
    {
        *this = *this * f;
        return *this;
    }

    constexpr tvec3<T> operator/(T f) const { return { x / f, y / f, z / f }; }
    constexpr tvec3<T>& operator/=(T f)
    {
        *this = *this / f;
        return *this;
    }
};

template<typename T>
constexpr tvec3<T> operator+(const tvec3<T>& v) { return v; }

template<typename T>
constexpr tvec3<T> operator-(const tvec3<T>& v) { return { -v.x, -v.y, -v.z }; }

template<typename T>
constexpr T dot(const tvec3<T>& lhs, const tvec3<T>& rhs)
{
    return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
}

template<typename T>
constexpr T length2(const tvec3<T>& v)
{
    return dot(v, v);
}

template<typename T, typename = typename std::enable_if<IsFloatingPoint<T>::value>::type>
constexpr T length(const tvec3<T>& v)
{
    return std::sqrt(length2(v));
}

template<typename T, typename = typename std::enable_if<IsFloatingPoint<T>::value>::type>
constexpr tvec3<T> normalize(const tvec3<T>& v)
{
    return v / length(v);
}

template<typename T>
constexpr tvec3<T> cross(const tvec3<T>& lhs, const tvec3<T>& rhs)
{
    return {
        lhs.y * rhs.z - lhs.z * rhs.y,
        lhs.z * rhs.x - lhs.x * rhs.z,
        lhs.x * rhs.y - lhs.y * rhs.x
    };
}

using vec3 = tvec3<f32>;
using dvec3 = tvec3<f64>;
using uvec3 = tvec3<u32>;
using ivec3 = tvec3<i32>;
//using bvec3 = tvec3<bool>; // TODO: We will need a specialization for this (if we even need it)!

template<typename T>
struct tvec4 {
    T x, y, z, w;

    constexpr tvec4(T x, T y, T z, T w)
        : x(x)
        , y(y)
        , z(z)
        , w(w)
    {
    }

    explicit constexpr tvec4(T e = VectorValue<T>::null)
        : tvec4(e, e, e, e)
    {
    }

    explicit constexpr tvec4(tvec3<T> v, T e = VectorValue<T>::unit)
        : tvec4(v.x, v.y, v.z, e)
    {
    }
};

using vec4 = tvec4<f32>;
using dvec4 = tvec4<f64>;
using uvec4 = tvec4<u32>;
using ivec4 = tvec4<i32>;
//using bvec4 = tvec4<bool>; // TODO: We will need a specialization for this (if we even need it)!

// Quaternion
// TODO: Add multiplication & vector transformations & to_matrix(quat)

template<typename T>
struct tquat {
    tvec3<T> xyz;
    T w;
};

using quat = tquat<f32>;
using dquat = tquat<f64>;

// Matrices
// TODO: Add multiplication & vector transformations

template<typename T>
struct mat3 {
    tvec3<T> x, y, z;
};

template<typename T>
struct mat4 {
    tvec4<T> x, y, z, w;

    tvec3<T> translation() const { return vec3(w); }
    void setTranslation(tvec3<T> t) { w = vec4(t); }
};

// Vector math constants

constexpr vec3 globalX = vec3(1, 0, 0);
constexpr vec3 globalY = vec3(0, 1, 0);
constexpr vec3 globalZ = vec3(0, 0, 1);

// NOTE: Using a y-up right-handed coordinate system
constexpr vec3 globalRight = +globalX;
constexpr vec3 globalUp = +globalY;
constexpr vec3 globalForward = -globalZ;

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
    float distance;
};

struct Triangle {
    vec3 v0, v1, v2;
};

// Coordinate system conversions
// TODO: vector to spherical, vice versa

// Random number generation
// TODO: randomInt, randomFloat, etc. make an object containing a random generator from std::random and expose some utility functions

} // namespace moos
