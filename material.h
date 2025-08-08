#ifndef MATERIAL_H
#define MATERIAL_H

#include "texture.h"
#include "utilities.h"

class material
{
    public:
        virtual ~material() = default;

        virtual color emitted(float u, float v, const vec3& p) const
        {
            return color(0,0,0);
        }

        virtual bool scatter(const vec3& dirIn, const vec3& normal, const vec3& uv, color& attenuation, vec3& scattered, uint32_t& seed) const
        {
            return false;
        }
};

class lambertian : public material
{
    public:
        lambertian(const color& a, shared_ptr<texture> t) : albedo(a), diffuse(t){}

        bool scatter(const vec3& dirIn, const vec3& normal, const vec3& uv, color& attenuation, vec3& scattered, uint32_t& seed) const override
        {
            vec3 scatteredDir = normal + randomUnitVector(seed);

            if(scatteredDir.nearZero())
                scatteredDir = normal;

            scattered = scatteredDir;
            attenuation = albedo * diffuse->value(uv.x(), uv.y(), vec3::up());
            return true;
        }

    private:
        color albedo;
        shared_ptr<texture> diffuse;
};

class diffuseLight : public material
{
    public:
        diffuseLight(const color& c) : emission(c) {}

        color emitted(float u, float v, const vec3& p) const override
        {
            return emission;
        }

    private:
        color emission;
};

#endif
