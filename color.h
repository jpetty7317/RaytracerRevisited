#ifndef COLOR_H
#define COLOR_H

#include "interval.h"
#include "vec3.h"

using color = vec3;

template <typename T>
inline T linearToGamma(const T linear)
{
    if(linear > 0)
        return sqrt(linear);

    return 0;
}

void writeColor (std::ofstream& out, const color& c)
{
    auto cr = linearToGamma(c.r());
    auto cg = linearToGamma(c.g());
    auto cb = linearToGamma(c.b());

    static const interval intensity {0.000, 0.999};
    int r = int(256 * intensity.clamp(cr));
    int g = int(256 * intensity.clamp(cg));
    int b = int(256 * intensity.clamp(cb));

    out << r << ' ' << g << ' ' << b << '\n';
}

#endif
