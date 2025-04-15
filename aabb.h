#ifndef AABB_H
#define AABB_H

#include "hittable.h"
#include "utilities.h"
#include <vector>

class aabb
{
public:
    aabb(){};

    // optimize to not have to copy hittable list
    aabb(const point3& min, const point3& max) : mMin{min}, mMax{max}, mSize{ max - min } {}

    point3& min() {return mMin;}
    point3& max()  {return mMax;}
    vec3& size() {return mSize;}

    /*bool hit(const ray& r, interval rayT) const
    {
        for(int i = 0; i < 3; i++)
        {
            float invDir = r.invDirection()[i];
            float t0 = (mMin[i] - r.origin()[i]) * invDir;
            float t1 = (mMax[i] - r.origin()[i]) * invDir;

            if(invDir < 0.0f)
                std::swap(t0, t1);

            rayT.min = t0 > rayT.min ? t0 : rayT.min;
            rayT.max = t1 < rayT.max ? t1 : rayT.max;

            if(rayT.max <= rayT.min)
                return false;
        }
        return true;
    }*/

    bool hit(const ray& r, interval rayT, hitRecord& rec) const
    {
        for(int i = 0; i < 3; i++)
        {
            float invDir = r.invDirection()[i];
            float t0 = (mMin[i] - r.origin()[i]) * invDir;
            float t1 = (mMax[i] - r.origin()[i]) * invDir;

            if(invDir < 0.0f)
                std::swap(t0, t1);

            rayT.min = t0 > rayT.min ? t0 : rayT.min;
            rayT.max = t1 < rayT.max ? t1 : rayT.max;

            if(rayT.max <= rayT.min)
                return false;
        }
        rec.t = rayT.min;
        return true;
    }

    bool intersects(aabb& b)
    {
        return (mMin.x() <= b.max().x()) && (mMax.x() >= b.min().x()) &&
                (mMin.y() <= b.max().y()) && (mMax.y() >= b.min().y()) &&
                (mMin.z() <= b.max().z()) && (mMax.z() >= b.min().z());
    }

    void grow(const point3& p)
    {
        mMin = vmin(mMin, p);
        mMax = vmax(mMax, p);
    }

    void grow(aabb& b)
    {
        if(b.min() == vec3::posInf())
            return;

        grow(b.min());
        grow(b.max());
    }

    float area()
    {
        vec3 e {mMax - mMin};
        return e.x() * e.y() + e.y() * e.z() + e.z() * e.x();
    }

private:
    point3 mMin{};
    point3 mMax{};
    vec3 mSize{};
};

#endif
