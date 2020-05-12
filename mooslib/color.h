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

#include <mooslib/math.h>

namespace moos {

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
        Float luminance(const vec3& color)
        {
            constexpr vec3 chromacity = vec3(0.2126, 0.7152, 0.0722);
            return dot(color, chromacity);
        }

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

} // namespace colorspace

} // namespace moos
