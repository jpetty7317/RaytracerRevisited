#ifndef VEC3_H
#define VEC3_H

#include "utilities.h"

class vec3 {
    private:
        float e[3] {};

    public:
        vec3(){}
        vec3(float  x) : e {x,x,x} {}
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

        bool nearZero() const
        {
            float s = 0.0000001f;
            return (std::fabs(e[0]) < s) && (std::fabs(e[1]) < s) && (std::fabs(e[2]) < s);
        }

        static glm::vec3 up() {return glm::vec3 {0.0, 1.0, 0.0};}
        static glm::vec3 right() {return glm::vec3 {1.0, 0.0, 0.0};} // Right handed coordinated (+x goes to the right)
        static glm::vec3 forward() {return glm::vec3{0.0, 0.0, -1.0};} // Right handed coordinates (-z goes into screen)
        static glm::vec3 posInf() {return glm::vec3{infinity, infinity, infinity};}
        static glm::vec3 negInf() {return glm::vec3{-infinity, -infinity, -infinity};}
};

// point alias for semantics
//using point3 = vec3;

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

inline glm::vec3 operator*(const glm::vec3& v1, const float v)
{
    return glm::vec3 {v1[0] * v, v1[1] * v, v1[2] * v};
}

inline glm::vec3 operator/(const glm::vec3& v1, const float v)
{
    return glm::vec3 {v1[0] / v, v1[1] / v, v1[2] / v};
}

inline bool operator==(const vec3& v1, const vec3& v2)
{
    return v1.x() == v2.x() && v1.y() == v2.y() && v1.z() == v2.z();
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

inline float randomValueNormalDistribution(uint32_t& seed)
{
    float theta = 2.0f * pi * randGen<float>(seed);
    float rho = std::sqrt(-2.0f * std::log(randGen<float>(seed)));
    return rho * std::cos(theta);
}

inline glm::vec3 randomUnitVector(uint32_t& seed)
{
    return glm::normalize(glm::vec3{
        randomValueNormalDistribution(seed),
        randomValueNormalDistribution(seed),
        randomValueNormalDistribution(seed),
    });
}

/*inline vec3 randomVectorOnHemisphere(const vec3& normal, uint32_t& seed)
{
    vec3 onUnitSphere = randomUnitVector(seed);

    return dot(onUnitSphere, normal) > 0.0 ? onUnitSphere : -onUnitSphere;
}*/

inline vec3 vmin(const vec3& a, const vec3& b)
{
    return vec3{fminf(a.x(), b.x()), fminf(a.y(), b.y()), fminf(a.z(), b.z())};
}

inline vec3 vmax(const vec3& a, const vec3& b)
{
    return vec3{fmaxf(a.x(), b.x()), fmaxf(a.y(), b.y()), fmaxf(a.z(), b.z())};
}
#endif
