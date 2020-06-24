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

#include <mooslib/core.h>
#include <mooslib/vector.h>

#include <random> // for random number generation

namespace moos {

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

    template<typename T = i32, ENABLE_IF_INTEGRAL(T)>
    T randomIntInRange(T minInclusive, T maxInclusive)
    {
        return std::uniform_int_distribution<T>(minInclusive, maxInclusive)(m_engine);
    }

    vec3 randomInXyUnitDisk()
    {
        vec3 position;
        do {
            position = vec3(randomFloatInRange(-1.0, +1.0),
                            randomFloatInRange(-1.0, +1.0),
                            0.0f);
        } while (length2(position) >= 1.0);
        return position;
    }

    vec3 randomInUnitSphere()
    {
        vec3 position;
        do {
            position = vec3(randomFloatInRange(-1.0, +1.0),
                            randomFloatInRange(-1.0, +1.0),
                            randomFloatInRange(-1.0, +1.0));
        } while (moos::length2(position) >= 1.0);
        return position;
    }

private:
    std::mt19937_64 m_engine;
};

} // namespace moos
