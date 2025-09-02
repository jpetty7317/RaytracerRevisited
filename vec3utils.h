#ifndef VEC3UTILS_H
#define VEC3UTILS_H

//#include "utilities.h"

inline glm::vec3 operator*(const glm::vec3& v1, const float v)
{
    return glm::vec3 {v1[0] * v, v1[1] * v, v1[2] * v};
}

inline glm::vec3 operator/(const glm::vec3& v1, const float v)
{
    return glm::vec3 {v1[0] / v, v1[1] / v, v1[2] / v};
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

#endif
