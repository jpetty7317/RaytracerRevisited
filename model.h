#ifndef HITTABLE_LIST_H
#define HITTABLE_LIST_H

#include "aabb.h"
#include "bvh.h"
#include "triangle.h"
#include "utilities.h"

#include <vector>

class model// : public hittable
{
    public:
        aabb bounds {};
        bvh mbvh {};

        std::vector<shared_ptr<triangle>> triangles;

        model(){}

        model(const glm::vec3& min, const glm::vec3& max) : bounds{min, max} {}

        void clear() { triangles.clear(); }

        void addTriangle(shared_ptr<triangle> object)
        {
            triangles.push_back(object);
        }
};

#endif
