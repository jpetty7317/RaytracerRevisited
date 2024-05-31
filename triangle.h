#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "hittable.h"
#include "utilities.h"


// TODO: Maybe figure out if the material can be known ahead of time. Knowing it's opaque means we can discard 
// back faces sooner, only having to resolve transluscant
class triangle : public hittable
{
    private:
        vec3 p0 {};
        vec3 p1 {};
        vec3 p2 {};

    public:

        triangle(const vec3& a, const vec3& b, const vec3& c): p0{a}, p1{b}, p2{c}{} 

        bool hit(const ray& r, interval rayT, hitRecord& rec) const override
        {
            constexpr float epsilon = std::numeric_limits<float>::epsilon();

            //Get tri edges
            vec3 e1 = p1 - p0;
            vec3 e2 = p2 - p0;
            vec3 normal = cross(e1, e2).normalize();

            if(dot(cross(e1, e2), r.direction()) > 0.0)
                return false;

            vec3 rCrossE2 = cross(r.direction(), e2);
            float det = dot(e1, rCrossE2);

            if(det < epsilon)
                return false;
            
            vec3 s = r.origin() - p0;
            float u = dot(s, rCrossE2);

            if(u < 0.0f || u > det)
                return false;

            vec3 sCrossE1 = cross(s, e1);
            float v = dot(r.direction(), sCrossE1);

            if(v < 0.0f || u + v > det)
                return false;

            float t = dot(e2, sCrossE1);

            float invDet = 1.0f / det;
            t *= invDet;
            u *= invDet;
            v *= invDet;

            if (t > rayT.max || t < rayT.min)
		        return false;

            if(t > epsilon)
            {
                rec.point = r.at(t);
                rec.normal = normal;
                rec.t = t;
                return true;
            }
            else
            {
                return false;
            }
        }
};

#endif
