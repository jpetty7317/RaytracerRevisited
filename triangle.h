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
        vec3 cent {};

    public:
        triangle(const vec3& a, const vec3& b, const vec3& c): p0{a}, p1{b}, p2{c}, cent{(a + b + c) * 0.33333f}{} 

        const point3& v0() const { return p0; }
        const point3& v1() const { return p1; }
        const point3& v2() const { return p2; }
        const point3& centroid() const { return cent; }

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

        void Project(const std::vector<point3>& points, const vec3& axis, interval& minMax)
        {
            for(auto& p : points)
            {
                float val = dot(axis, p);
                if(val < minMax.min) minMax.min = val;
                if(val > minMax.max) minMax.max = val;
            }
        }

        bool intersectsAABB(aabb& box)
        {
            std::vector<vec3> boxNormals {{1,0,0},
                                            {0,1,0},
                                            {0,0,1}};

            interval boxMinMax;
            interval triMinMax;
            for(int i = 0; i < 3; i++)
            {
                Project({p0, p1, p2}, boxNormals[i], boxMinMax);
                if(boxMinMax.max < box.min()[i] || boxMinMax.min > box.max()[i])
                {
                    //std::cout << 1 << " " << i << '\n';
                    return false;
                }
            }

            //Get tri edges
            vec3 e1 = p1 - p0;
            vec3 e2 = p2 - p0;
            vec3 normal = cross(e1, e2).normalize();
            float triOffset = dot(normal, p0);
            std::vector<point3> boxVerts {{box.min()},
                                            {box.max()},
                                            {box.min() + vec3{1,0,0} * box.size().x()},
                                            {box.min() + vec3{0,1,0} * box.size().x()},
                                            {box.min() + vec3{0,0,1} * box.size().x()},
                                            {box.max() - vec3(1,0,0) * box.size().x()},
                                            {box.max() - vec3(0,1,0) * box.size().x()},
                                            {box.max() - vec3(0,0,1) * box.size().x()}};
            Project(boxVerts, normal, triMinMax);
            if(triMinMax.max < triOffset || triMinMax.min > triOffset)
            {
                //std::cout << 2 << '\n';
                return false;
            }

            std::vector<vec3> triEdges = {{p0 - p1},
                                            {p1 - p2},
                                            {p2 - p0}};
            
            for(int i = 0; i < 3; i++)
            {
                for(int j = 0; j < 3; j++)
                {
                    vec3 axis {cross(triEdges[i], boxNormals[i])};
                    Project(boxVerts, axis, boxMinMax);
                    Project({p0, p1, p2}, axis, triMinMax);
                    if(boxMinMax.max <= triMinMax.min || boxMinMax.min >= triMinMax.max)
                    {
                        //std::cout << 3 << " " << i*j << '\n';
                        return false;
                    }
                }
            }

            //std::cout << 4 << '\n';

            return true;
        }
};

#endif
