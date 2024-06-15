#ifndef HITTABLE_LIST_H
#define HITTABLE_LIST_H

#include "aabb.h"
#include "triangle.h"
#include "utilities.h"

#include <vector>

class model : public hittable
{
    public:
        aabb bounds {};

        std::vector<shared_ptr<triangle>> triangles;

        model(){}

        model(const point3& min, const point3& max) : bounds{min, max} {}

        void clear() { triangles.clear(); }

        void addTriangle(shared_ptr<triangle> object)
        {
            triangles.push_back(object);
        }

        bool hit(const ray& r, interval rayT, hitRecord& rec) const override
        {
            if(!bounds.hit(r, rayT))
                return false;

            hitRecord tempRec;
            bool hitAnything = false;
            float closestSoFar = rayT.max;

            for(const auto& triangle: triangles)
            {
                if(triangle->hit(r, interval{rayT.min, closestSoFar}, tempRec))
                {
                    hitAnything = true;
                    closestSoFar = tempRec.t;
                    rec = tempRec;
                }
            }

            return hitAnything;
        }
};

#endif
