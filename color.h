#ifndef COLOR_H
#define COLOR_H

#include "interval.h"
#include <glm/vec3.hpp>

template <typename T>
inline T linearToGamma(const T linear)
{
    if(linear > 0)
        return sqrt(linear);

    return 0;
}

void writeColor (std::ofstream& out, const glm::vec3& c)
{
    auto cr = c.r;
    auto cg = c.g;
    auto cb = c.b;

    if (cr != cr) cr = 0.0f;
    if (cg != cg) cg = 0.0f;
    if (cb != cb) cb = 0.0f;

    cr = linearToGamma(c.r);
    cg = linearToGamma(c.g);
    cb = linearToGamma(c.b);

    static const interval intensity {0.000, 0.999};
    int r = int(256 * intensity.clamp(cr));
    int g = int(256 * intensity.clamp(cg));
    int b = int(256 * intensity.clamp(cb));

    out << r << ' ' << g << ' ' << b << '\n';
}

#endif
