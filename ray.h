#ifndef RAY_H
#define RAY_H

#include "vec3.h"

class ray
{
private:
    point3 orig {};
    vec3 dir {};
    vec3 invDir {};

public:
    float t = infinity;
    vec3 normal{};

    ray (): orig{0,0,0}, dir{0,0,0} {};
    ray (const point3& o, const vec3& d) : orig (o), dir (d), invDir{ 1.0f / d[0], 1.0f / d[1], 1.0f / d[2] } {}
    
    const point3& origin() const { return orig; }
    const vec3& direction() const { return dir; }
    const vec3& invDirection() const { return invDir; }

    point3 at(float t) const
    {
        return orig + t * dir;
    }
};

#endif
