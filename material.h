#ifndef MATERIAL_H
#define MATERIAL_H

#include "texture.h"
#include "vec3utils.h"

class material
{
    public:
        virtual ~material() = default;

        virtual glm::vec3 emitted(float u, float v, const glm::vec3& p) const
        {
            return glm::vec3(0,0,0);
        }

        virtual bool scatter(const glm::vec3& dirIn, const glm::vec3& normal, const glm::vec3& uv, glm::vec3& attenuation, glm::vec3& scattered, uint32_t& seed) const
        {
            return false;
        }
};

class lambertian : public material
{
    public:
        lambertian(const glm::vec3& a, shared_ptr<texture> t) : albedo(a), diffuse(t){}

        bool scatter(const glm::vec3& dirIn, const glm::vec3& normal, const glm::vec3& uv, glm::vec3& attenuation, glm::vec3& scattered, uint32_t& seed) const override
        {
            glm::vec3 scatteredDir = normal + randomUnitVector(seed);

            if(nearZero(scatteredDir))
                scatteredDir = normal;

            scattered = scatteredDir;
            attenuation = albedo * diffuse->value(uv.x, uv.y, VEC3_UP);
            return true;
        }

    private:
        glm::vec3 albedo;
        shared_ptr<texture> diffuse;
};

class diffuseLight : public material
{
    public:
        diffuseLight(const glm::vec3& c) : emission(c) {}

        glm::vec3 emitted(float u, float v, const glm::vec3& p) const override
        {
            return emission;
        }

    private:
        glm::vec3 emission;
};

#endif
