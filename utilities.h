#ifndef UTILITIES_H
#define UTILITIES_H

#include <cmath>
#include <iostream>
#include <fstream>
#include <limits>
#include <memory>

// C++ std usings

using std::make_shared;
using std::shared_ptr;
using std::sqrt;

// Constants

const float infinity = std::numeric_limits<float>::infinity();
const float pi = 3.1415926535897932385;

// Utility functions

inline float degreesToRadians(float degrees)
{
    return degrees * pi / 180.0f;
}

// Common headers

#include "color.h"
#include "ray.h"
#include "vec3.h"

#endif
