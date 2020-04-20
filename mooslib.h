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

#include <cstdint> // for integer definitions
#include <limits>  // for numeric min/max values etc.
#include <utility> // for std::move

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

// Common functions & macros

template<typename T>
const auto move = std::move<T>;

// TODO: Define a custom assert!

// Basic file reading operations
// TODO: Read entire file into a string, etc.

// Math constants & basic math functions

constexpr float E = 2.718281828459f;
constexpr float PI  = 3.141592653590f;
constexpr float TWO_PI = 2.0f * PI;

template<typename T>
inline constexpr T square(T x)
{
    return x * x;
}

inline constexpr float radians(float degrees)
{
    return degrees / 180.0f * PI;
}

inline constexpr float degrees(float radians)
{
	return radians / PI * 180.0f;
}

// Vectors & quaternions & matrices
// TODO!

struct vec2 {};
struct vec3 {};
struct vec4 {};

struct ivec2 {};
struct ivec3 {};
struct ivec4 {};

struct uvec2 {};
struct uvec3 {};
struct uvec4 {};

struct bvec2 {};
struct bvec3 {};
struct bvec4 {};

struct quat {};

struct mat3 {};
struct mat4 {};

// Vector math constants

constexpr vec3 globalX = vec3(1, 0, 0);
constexpr vec3 globalY = vec3(0, 1, 0);
constexpr vec3 globalZ = vec3(0, 0, 1);

// NOTE: Using a y-up right-handed coordinate system
constexpr vec3 globalRight   = +globalX;
constexpr vec3 globalUp      = +globalY;
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
