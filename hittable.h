#ifndef HITTABLE_H
#define HITTABLE_H

#include "utilities.h"

class hitRecord 
{
    public:
        point3 point {};
        vec3 normal {};
        float t = 0.0f;

        ///Currently removing this as it makes some assumptions on working in the interior of objects and for now
        ///erroneously flipping correctly calculating front facing normals.
        ///This may come back up as being properly important if we get to refractive materials and the like.
        // Adding this for now, but want to revisit on a more material basis
        //bool frontFace = false;

        // Not sure if I like this. Negates calculate normal if it's determined to point inward
        // Since we'll be processing full meshes at some point this will be less of a problem
        // and we probably get rid of this altogether.
        //void setFaceNormal(const ray& r, const vec3& outNormal)
        //{
        //    frontFace = dot(r.direction(), normal) < 0.0f;
        //    normal = frontFace ? outNormal : -outNormal;
        //}
};

class hittable
{
    public:
        virtual ~hittable() = default;

        virtual bool hit(const ray& r, interval rayT, hitRecord& rec) const = 0;
};

#endif
