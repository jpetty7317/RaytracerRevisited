#ifndef AABB_H
#define AABB_H

#include "hittable.h"
#include "hittablelist.h"
#include "utilities.h"
#include <vector>

class aabb : public hittable {
public:
    aabb(){};

    // optimize to not have to copy hittable list
    aabb(const point3& min, const point3& max, hittableList model) : mMin{min}, mMax{max}, mModel{model} {}

    const point3& min() const {return mMin;}
    const point3& max() const {return mMax;}
    const hittableList& model() const { return mModel; }

    bool hit(const ray& r, interval rayT, hitRecord& rec) const override
    {
        for(int i = 0; i < 3; i++)
        {
            float invDir = 1.0f / r.direction()[i];
            float t0 = (mMin[i] - r.origin()[i]) * invDir;
            float t1 = (mMax[i] - r.origin()[i]) * invDir;

            if(invDir < 0.0f)
                std::swap(t0, t1);

            rayT.min = t0 > rayT.min ? t0 : rayT.min;
            rayT.max = t1 < rayT.max ? t1 : rayT.max;

            if(rayT.max <= rayT.min)
                return false;
        }
        return model().hit(r, rayT, rec);
    }

private:
    point3 mMin{};
    point3 mMax{};

    hittableList mModel;
};

#endif
