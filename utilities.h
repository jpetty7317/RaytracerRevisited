#ifndef UTILITIES_H
#define UTILITIES_H

#include <cmath>
#include <iostream>
#include <fstream>
#include <limits>
#include <memory>
#include <random>

// C++ std usings

using std::make_shared;
using std::shared_ptr;
using std::sqrt;

// Constants

constexpr float infinity = std::numeric_limits<float>::infinity();
constexpr float pi = 3.1415926535897932385f;
constexpr float pi180 = pi / 180.0f;

// Utility functions

template <typename T>
inline T degreesToRadians(T degrees)
{
    return degrees * pi180;
}

template <typename T>
inline T randGen()
{
    static thread_local std::uniform_real_distribution<T> distribution(0.0, 1.0);
    static thread_local std::mt19937_64 generator;
    return distribution(generator);
}

template <typename T, typename U>
inline T randGen(T min, U max)
{
    return min + (max - min) * randGen<T>();
}

// Common headers

#include "color.h"
#include "interval.h"
#include "ray.h"
#include "vec3.h"

#endif
