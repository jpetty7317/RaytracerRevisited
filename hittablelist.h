#ifndef HITTABLE_LIST_H
#define HITTABLE_LIST_H

#include "hittable.h"
#include "utilities.h"

#include <vector>

class hittableList : public hittable
{
    public:
        std::vector<shared_ptr<hittable>> objects;

        hittableList(){}
        hittableList(shared_ptr<hittable> object) { }

        void clear() { objects.clear(); }

        void addObject(shared_ptr<hittable> object)
        {
            objects.push_back(object);
        }

        bool hit(const ray& r, interval rayT, hitRecord& rec) const override
        {
            hitRecord tempRec;
            bool hitAnything = false;
            float closestSoFar = rayT.max;

            for(const auto& object: objects)
            {
                if(object->hit(r, interval{rayT.min, closestSoFar}, tempRec))
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
