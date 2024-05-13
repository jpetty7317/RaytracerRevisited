#ifndef COLOR_H
#define COLOR_H

#include <fstream>
#include "vec3.h"

using color = vec3;

void writeColor (std::ofstream& out, const color& c)
{
    int r = int(255.999 * c.r());
    int g = int(255.999 * c.g());
    int b = int(255.999 * c.b());

    out << r << ' ' << g << ' ' << b << '\n';
}

#endif