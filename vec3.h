#ifndef VEC3_H
#define VEC3_H

#include "utilities.h"

class vec3 {
    private:
        float e[3] {};

    public:
        vec3(){}
        vec3(const vec3& v) : e{v.x(), v.y(), v.z()}{};
        vec3(float x, float y, float z): e {x, y ,z}{};

        // For vector semnatics 
        float x() const { return e[0]; }
        float y() const { return e[1]; }
        float z() const { return e[2]; }

        // For color semantics
        float r() const { return e[0]; }
        float g() const { return e[1]; }
        float b() const { return e[2]; }

        vec3 operator-() const { return vec3(-e[0], -e[1], -e[2]); }
        float operator[](int i) const { return e[i]; }
        float& operator[](int i) { return e[i]; }

        vec3& operator+=(const vec3 &v2)
        {
            e[0] += v2.e[0];
            e[1] += v2.e[1];
            e[2] += v2.e[2];
            return *this;
        }

        vec3& operator-=(const vec3 &v2)
        {
            e[0] -= v2.e[0];
            e[1] -= v2.e[1];
            e[2] -= v2.e[2];
            return *this;
        }

        vec3& operator*=(const vec3 &v2)
        {
            e[0] *= v2.e[0];
            e[1] *= v2.e[1];
            e[2] *= v2.e[2];
            return *this;
        }

        vec3& operator*=(const float t)
        {
            e[0] *= t;
            e[1] *= t;
            e[2] *= t;
            return *this;
        }

        vec3& operator/=(const float t)
        {
            e[0] /= t;
            e[1] /= t;
            e[2] /= t;
            return *this;
        }

        float squaredLength() const 
        {
            return e[0]*e[0] + e[1]*e[1] + e[2]*e[2];
        }

        float length() const 
        {
            return sqrt(squaredLength());
        }

        vec3& normalize()
        {
            return *this /= length();
        }

        static vec3 up() {return vec3 {0.0, 1.0, 0.0};} 
        static vec3 right() {return vec3 {1.0, 0.0, 0.0};} // Right handed coordinated (+x goes to the right)
        static vec3 forward() {return vec3{0.0, 0.0, -1.0};} // Right handed coordinates (-z goes into screen)
};

// point alias for semantics
using point3 = vec3;

// utility functions
inline std::ostream& operator<<(std::ostream& out, const vec3& v)
{
    return out << v.x() << ' ' << v.y() << ' ' << v.z();
}

inline vec3 operator+(const vec3& v1, const vec3& v2)
{
    return vec3 {v1[0] + v2[0], v1[1] + v2[1], v1[2] + v2[2]};
}

inline vec3 operator-(const vec3& v1, const vec3& v2)
{
    return vec3 {v1[0] - v2[0], v1[1] - v2[1], v1[2] - v2[2]};
}

inline vec3 operator*(const vec3& v1, const vec3& v2)
{
    return vec3 {v1[0] * v2[0], v1[1] * v2[1], v1[2] * v2[2]};
}

inline vec3 operator*(const float v, const vec3& v1)
{
    return vec3 {v1[0] * v, v1[1] * v, v1[2] * v};
}

inline vec3 operator*(const vec3& v1, const float v)
{
    return vec3 {v1[0] * v, v1[1] * v, v1[2] * v};
}

inline vec3 operator/(const vec3& v1, const float v)
{
    return vec3 {v1[0] / v, v1[1] / v, v1[2] / v};
}

inline float dot(const vec3& v1, const vec3& v2)
{
    return  v1[0] * v2[0]
          + v1[1] * v2[1]
          + v1[2] * v2[2];
}

inline vec3 cross(const vec3& v1, const vec3& v2)
{
    return vec3 {   v1[1] * v2[2] - v1[2] * v2[1],
                    v1[2] * v2[0] - v1[0] * v2[2],
                    v1[0] * v2[1] - v1[1] * v2[0]   };
}

inline vec3 randomInUnitSphere()
{
    vec3 v {};
    do
    {
        v = 2.0f * vec3(randGen<float>(), randGen<float>(), randGen<float>());
    } while (v.squaredLength() >= 1.0);

    return v;
}

inline vec3 randomUnitVector()
{
    return randomInUnitSphere().normalize();
}

inline vec3 randomVectorOnHemisphere(const vec3& normal)
{
    vec3 onUnitSphere = randomUnitVector();

    return dot(onUnitSphere, normal) > 0.0 ? onUnitSphere : -onUnitSphere;
}

#endif
