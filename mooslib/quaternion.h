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

#include "core.h"
#include "matrix.h"
#include "vector.h"

namespace moos {

template<typename T, typename _ = void>
struct tquat {
};

template<typename T>
struct tquat<T, ENABLE_STRUCT_IF_FLOATING_POINT(T)> {
    tvec3<T> vec;
    T w;

    constexpr tquat(tvec3<T> vec, T w) noexcept
        : vec(vec)
        , w(w)
    {
    }

    constexpr tquat() noexcept
        : tquat({ static_cast<T>(0), static_cast<T>(0), static_cast<T>(0) }, static_cast<T>(1))
    {
    }

    constexpr tquat<T> operator*(const tquat<T>& q) const
    {
        const tquat<T>& p = *this;
        return {
            p.w * q.vec + q.w * p.vec + cross(p.vec, q.vec),
            p.w * q.w - dot(p.vec, q.vec)
        };
    }

    constexpr tquat<T>& operator*=(const tquat<T>& q) const
    {
        *this = *this * q;
        return this;
    }

    constexpr tvec3<T> operator*(const tvec3<T>& v) const
    {
        // Method by Fabian 'ryg' Giessen who posted it on some now defunct forum. There is some info
        // at https://blog.molecular-matters.com/2013/05/24/a-faster-quaternion-vector-multiplication/.

        tvec3<T> t = static_cast<T>(2) * cross(vec, v);
        tvec3<T> res = v + w * t + cross(vec, t);

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

template<typename T, ENABLE_IF_FLOATING_POINT(T)>
constexpr tmat4<T> quatToMatrix(const tquat<T>& q)
{
    // This function is a rewritten version of mat4x4_from_quat from https://github.com/datenwolf/linmath.h

    const T& a = q.w;
    const T& b = q.vec.x;
    const T& c = q.vec.y;
    const T& d = q.vec.z;
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

template<typename T, ENABLE_IF_FLOATING_POINT(T)>
constexpr tquat<T> quatFromMatrix(const tmat4<T>& m)
{
    // This function is a rewritten version of Mike Day's "Converting a Rotation Matrix to a Quaternion" code. Probably not official,
    // but a copy of the document can be found at https://d3cw3dd2w32x2b.cloudfront.net/wp-content/uploads/2015/01/matrix-to-quat.pdf

    const T& m00 = m.x.x;
    const T& m11 = m.y.y;
    const T& m22 = m.z.z;

    tquat<T> q;
    T t = static_cast<T>(1);

    if (m22 < static_cast<T>(0)) {
        if (m00 > m11) {
            t += +m00 - m11 - m22;
            q = { { t, m.x.y + m.y.x, m.z.x + m.x.z }, m.y.z - m.z.y };
        } else {
            t += -m00 + m11 - m22;
            q = { { m.x.y + m.y.x, t, m.y.z + m.z.y }, m.z.x - m.x.z };
        }
    } else {
        if (m00 < -m11) {
            t += -m00 - m11 + m22;
            q = { { m.z.x + m.x.z, m.y.z + m.z.y, t }, m.x.y - m.y.x };
        } else {
            t += +m00 + m11 + m22;
            q = { { m.y.z - m.z.y, m.z.x - m.x.z, m.x.y - m.y.x }, t };
        }
    }

    T scale = static_cast<T>(0.5) / std::sqrt(t);
    q.vec *= scale;
    q.w *= scale;

    return q;
}

using quat = tquat<Float>;
using fquat = tquat<f32>;
using dquat = tquat<f64>;

} // namespace moos
