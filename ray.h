#ifndef RAY_H
#define RAY_H

#include "utilities.h"
#include "material.h"

class ray
{
private:
    glm::vec3 orig {};
    glm::vec3 dir {};
    glm::vec3 invDir {};

public:
    float t = infinity;
    glm::vec3 normal{};
    glm::vec3 uv{};
    shared_ptr<material> mat;

    ray (): orig{0,0,0}, dir{0,0,0} {};
    ray (const glm::vec3& o, const glm::vec3& d) : orig (o), dir (d), invDir{ 1.0f / d[0], 1.0f / d[1], 1.0f / d[2] } {}
    
    const glm::vec3& origin() const { return orig; }
    const glm::vec3& direction() const { return dir; }
    const glm::vec3& invDirection() const { return invDir; }

    glm::vec3 at(float t) const
    {
        return orig + t * dir;
    }
};

#endif
