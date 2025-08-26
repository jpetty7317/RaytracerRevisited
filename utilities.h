#ifndef UTILITIES_H
#define UTILITIES_H

#include <cmath>
#include <iostream>
#include <fstream>
#include <limits>
#include <memory>
#include <glm/glm.hpp>
#include <random>

// C++ std usings

using std::make_shared;
using std::shared_ptr;
using std::sqrt;

// Constants

constexpr float infinity = 1e30;
constexpr float pi = 3.1415926535897932385f;
constexpr float pi180 = pi / 180.0f;

// Utility functions

template <typename T>
inline T degreesToRadians(T degrees)
{
    return degrees * pi180;
}

inline uint32_t randFast(uint32_t& seed)
{
    seed = seed * 747796485u + 2981336453u;
    uint32_t word = ((seed >> ((seed >> 28u) + 4u)) ^ seed) + 277803737;
    return (word >> 22u) ^ word;
}

template <typename T>
inline T randGen(uint32_t& seed)
{
    return (T)randFast(seed) / (T)std::numeric_limits<uint32_t>::max();
}

template <typename T, typename U>
inline T randGen(T min, U max, uint32_t& seed)
{
    return min + (max - min) * randGen<T>(seed);
}

inline bool nearZero(glm::vec3& v)
{
    float s = 0.0000001f;
    return (std::fabs(v[0]) < s) && (std::fabs(v[1]) < s) && (std::fabs(v[2]) < s);
}

// Common headers
#include "color.h"
#include "interval.h"
#include "ray.h"
#include "vec3.h"

#endif
