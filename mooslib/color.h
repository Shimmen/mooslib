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

#include "matrix.h"

namespace moos {

// NOTE: There is no one standard for this, but I will use this range
constexpr Float visibleLightMinWavelength = 380.0;
constexpr Float visibleLightMaxWavelength = 780.0;
constexpr Float visibleLightWavelengthRangeLength = visibleLightMaxWavelength - visibleLightMinWavelength;
constexpr size_t visibleLightWavelengthRangeSteps = static_cast<size_t>(visibleLightWavelengthRangeLength) + 1;

namespace standardIlluminant {
    constexpr Float D65 = 6504.0;
}

namespace colorTemperature {
    // From or inspired by https://en.wikipedia.org/wiki/Color_temperature#Categorizing_different_lighting
    constexpr Float candle = 1850.0;
    constexpr Float incandescentBulb = 2400.0;
    constexpr Float studioLight = 3200.0;
    constexpr Float fluorescentBulb = 5000.0;
}

Float blackBodyRadiation(Float temperature, Float wavelengthInNanometer)
{
    // From https://www.shadertoy.com/view/MstcD7 but it's also trivial to
    // reconstruct from Planck's Law: https://en.wikipedia.org/wiki/Planck%27s_law

    Float h = 6.6e-34; // Planck constant
    Float kb = 1.4e-23; // Boltzmann constant
    Float c = 3e8; // Speed of light

    Float w = wavelengthInNanometer / 1e9;
    const Float& t = temperature;

    Float w5 = w * w * w * w * w;
    Float o = 2.0 * h * (c * c) / (w5 * (std::exp((h * c) / (w * kb * t)) - 1.0));

    return o;
}

namespace colorspace {

    namespace XYZ {

        // Assuming 1931 standard observer

        // xyz (bar) fits from Listing 1 of https://research.nvidia.com/publication/simple-analytic-approximations-cie-xyz-color-matching-functions

        Float xBarFit(Float wave)
        {
            Float t1 = (wave - 442.0) * ((wave < 442.0) ? 0.0624 : 0.0374);
            Float t2 = (wave - 599.8) * ((wave < 599.8) ? 0.0264 : 0.0323);
            Float t3 = (wave - 501.1) * ((wave < 501.1) ? 0.0490 : 0.0382);
            return 0.362 * std::exp(-0.5 * t1 * t1) + 1.056 * std::exp(-0.5 * t2 * t2) - 0.065 * std::exp(-0.5 * t3 * t3);
        }

        Float yBarFit(Float wave)
        {
            Float t1 = (wave - 568.8) * ((wave < 568.8) ? 0.0213 : 0.0247);
            Float t2 = (wave - 530.9) * ((wave < 530.9) ? 0.0613 : 0.0322);
            return 0.821 * std::exp(-0.5 * t1 * t1) + 0.286 * std::exp(-0.5 * t2 * t2);
        }

        Float zBarFit(Float wave)
        {
            Float t1 = (wave - 437.0) * ((wave < 437.0) ? 0.0845 : 0.0278);
            Float t2 = (wave - 459.0) * ((wave < 459.0) ? 0.0385 : 0.0725);
            return 1.217 * std::exp(-0.5 * t1 * t1) + 0.681 * std::exp(-0.5 * t2 * t2);
        }

        Float photometricCurveFit(Float wave)
        {
            return yBarFit(wave);
        }

        vec3 fromSingleWavelength(Float power, Float wavelength)
        {
            Float X = xBarFit(wavelength);
            Float Y = yBarFit(wavelength);
            Float Z = zBarFit(wavelength);
            return power * vec3(X, Y, Z);
        }

        vec3 fromBlackBodyTemperature(Float temperature, int numSteps = 100)
        {
            Float stepWidth = visibleLightWavelengthRangeLength / static_cast<Float>(numSteps);

            vec3 XYZ = { 0, 0, 0 };
            for (int i = 0; i < numSteps; i++) {
                Float mix = static_cast<Float>(i) / static_cast<Float>(numSteps - 1);
                Float wavelength = lerp(visibleLightMinWavelength, visibleLightMaxWavelength, mix);
                Float power = blackBodyRadiation(temperature, wavelength);
                XYZ += fromSingleWavelength(power, wavelength) * stepWidth;
            }

            return XYZ;
        }

        vec3 from_xyY(vec2 xy, Float Y)
        {
            Float scale = Y / xy.y;
            Float X = scale * xy.x;
            Float Z = scale * (1.0 - xy.x - xy.y);
            return { X, Y, Z };
        }

        vec2 to_xy(vec3 XYZ)
        {
            Float sum = XYZ.x + XYZ.y + XYZ.z;
            Float x = XYZ.x / sum;
            Float y = XYZ.y / sum;
            return { x, y };
        }

        vec3 to_xyz(vec3 XYZ)
        {
            vec2 xy = to_xy(XYZ);
            Float z = 1.0 - xy.x - xy.y;
            return { xy.x, xy.y, z };
        }

    } // namespace XYZ

    const mat3 XYZ_from_sRGB = mat3(
        { 0.4123865632529917, 0.21263682167732384, 0.019330620152483987 },
        { 0.35759149092062537, 0.7151829818412507, 0.11919716364020845 },
        { 0.18045049120356368, 0.07218019648142547, 0.9503725870054354 });

    const mat3 sRGB_from_XYZ = mat3(
        { 3.2410032329763587, -0.9692242522025166, 0.055639419851975444 },
        { -1.5373989694887855, 1.875929983695176, -0.20401120612390997 },
        { -0.4986158819963629, 0.041554226340084724, 1.0571489771875335 });

    namespace sRGB {

        Float luminance(const vec3& color)
        {
            constexpr vec3 Y = vec3(0.2126, 0.7152, 0.0722);
            return dot(color, Y);
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

        vec3 fromBlackBodyTemperature(Float temperature, int numSteps = 100)
        {
            vec3 XYZ = XYZ::fromBlackBodyTemperature(temperature, numSteps);
            vec3 sRGB = sRGB_from_XYZ * XYZ;
            return sRGB;
        }

    } // namespace sRGB

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

        vec3 referenceToneMap(vec3 color)
        {
            color = inputMatrix * color;
            color = RRTAndODTFit(color);
            color = outputMatrix * color;
            color = clamp(color, vec3(0.0), vec3(1.0));
            return color;
        }

    } // namespace ACES

    namespace HSV {

        vec3 toRGB(const vec3& HSV)
        {
            // From https://en.wikipedia.org/wiki/HSL_and_HSV#HSV_to_RGB

            // make sure H is in range [0, 360] degrees
            Float h = HSV.x;
            h = std::fmod(h, 360.0);
            h += 360.0;
            h = std::fmod(h, 360.0);

            const Float& s = HSV.y;
            const Float& v = HSV.z;

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

        vec3 fromRGB(vec3 RGB)
        {
            // From https://en.wikipedia.org/wiki/HSL_and_HSV#From_RGB

            RGB = clamp(RGB, vec3(0.0), vec3(1.0));
            const Float& r = RGB.x;
            const Float& g = RGB.y;
            const Float& b = RGB.z;

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

    } // namespace HSV

} // namespace colorspace

} // namespace moos
