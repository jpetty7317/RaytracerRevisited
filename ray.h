#ifndef RAY_H
#define RAY_H

#include "vec3.h"

class ray
{
private:
    point3 orig {};
    vec3 dir {};

public:
    ray (): orig{0,0,0}, dir{0,0,0} {};
    ray (const point3& o, const vec3& d) : orig (o), dir (d) {}

    const point3& origin() const { return orig; }
    const vec3& direction() const {return dir; }

    point3 at(float t) const
    {
        return orig + t * dir;
    }
};

#endif