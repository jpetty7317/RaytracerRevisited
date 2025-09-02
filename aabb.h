#ifndef AABB_H
#define AABB_H

#include "utilities.h"
#include <vector>

class aabb
{
public:
    aabb(){};

    // optimize to not have to copy hittable list
    aabb(const glm::vec3& min, const glm::vec3& max) : mMin{min}, mMax{max}, mSize{ max - min } {}

    glm::vec3& min() {return mMin;}
    glm::vec3& max()  {return mMax;}
    glm::vec3& size() {return mSize;}

    float hit(const ray& r)
    {
        float tx1 = (mMin.x - r.origin().x) * r.invDirection().x;
        float tx2 = (mMax.x - r.origin().x) * r.invDirection().x;
        float tMin = std::min(tx1, tx2);
        float tMax = std::max(tx1, tx2);

        float ty1 = (mMin.y - r.origin().y) * r.invDirection().y;
        float ty2 = (mMax.y - r.origin().y) * r.invDirection().y;
        tMin = std::max(tMin, std::min(ty1, ty2));
        tMax = std::min(tMax, std::max(ty1, ty2));

        float tz1 = (mMin.z - r.origin().z) * r.invDirection().z;
        float tz2 = (mMax.z - r.origin().z) * r.invDirection().z;
        tMin = std::max(tMin, std::min(tz1, tz2));
        tMax = std::min(tMax, std::max(tz1, tz2));

        if(tMax >= tMin && tMin < r.t && tMax > 0.0f)
            return tMin;
        else
            return infinity;
    }

    bool intersects(aabb& b)
    {
        return (mMin.x <= b.max().x) && (mMax.x >= b.min().x) &&
                (mMin.y <= b.max().y) && (mMax.y >= b.min().y) &&
                (mMin.z <= b.max().z) && (mMax.z >= b.min().z);
    }

    void grow(const glm::vec3& p)
    {
        mMin = glm::min(mMin, p);
        mMax = glm::max(mMax, p);
    }

    void grow(aabb& b)
    {
        if(b.min() == VEC3_POSINF)
            return;

        grow(b.min());
        grow(b.max());
    }

    float area()
    {
        glm::vec3 e {mMax - mMin};
        return e.x * e.y + e.y * e.z + e.z * e.x;
    }

private:
    glm::vec3 mMin{};
    glm::vec3 mMax{};
    glm::vec3 mSize{};
};

#endif
