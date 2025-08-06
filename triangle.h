#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "utilities.h"
#include "material.h"


// TODO: Maybe figure out if the material can be known ahead of time. Knowing it's opaque means we can discard 
// back faces sooner, only having to resolve transluscant
class triangle
{
    private:
        vec3 p0 {};
        vec3 p1 {};
        vec3 p2 {};
        vec3 n0 {};
        vec3 n1 {};
        vec3 n2 {};
        vec3 uv0 {};
        vec3 uv1 {};
        vec3 uv2 {};
        vec3 cent {};
        shared_ptr<material> mat;

    public:
        triangle(const vec3& a, const vec3& b, const vec3& c,
                    const vec3& x, const vec3& y, const vec3& z,
                    const vec3& u0, const vec3& u1, const vec3& u2,
                    shared_ptr<material> m):
                        p0{a}, p1{b}, p2{c}, cent{(a + b + c) * 0.33333f},
                        n0{x}, n1{y}, n2{z}, mat{m},
                        uv0{u0}, uv1{u1}, uv2{u2}{}

        const point3& v0() const { return p0; }
        const point3& v1() const { return p1; }
        const point3& v2() const { return p2; }
        const point3& centroid() const { return cent; }

        void hit(ray& r)
        {
            const vec3 e1 = p1 - p0;
            const vec3 e2 = p2 - p0;

            const vec3 normal = (n0 + n1 + n2) / 3.0f;
            if(dot(r.direction(), normal) > 0.0)
                return;

            const vec3 h = cross(r.direction(), e2);
            const float a = dot(h, e1);
            if(a > -0.00001f && a < 0.00001f)
                return;
            
            const float f = 1.0f / a;
            const vec3 s = r.origin() - p0;
            const float u = f * dot(s, h);
            if(u < 0.0f || u > 1.0f)
                return;

            const vec3 q = cross(s, e1);
            const float v = f * dot(r.direction(), q);
            if(v < 0.0f || u + v > 1.0f)
                return;

            const float t = f * dot(e2, q);
            if(t > 0.00001f && t < r.t)
            {
                r.t = t;
                float w = 1.0f - u - v;
                r.normal = u * n1 + v * n2 + w * n0;
                r.uv = u * uv1 + v * uv2 + w * uv0;
                float x = r.uv.x() - std::floor(r.uv.x());
                float y = r.uv.y() - std::floor(r.uv.y());
                r.uv = vec3{x, y, 0.0f};
                r.mat = mat;
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
