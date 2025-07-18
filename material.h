#ifndef MATERIAL_H
#define MATERIAL_H

#include "utilities.h"

class material
{
    public:
        virtual ~material() = default;

        virtual color emitted(float u, float v, const vec3& p) const
        {
            return color(0,0,0);
        }

        virtual bool scatter(const vec3& dirIn, const vec3& normal, color& attenuation, vec3& scattered) const
        {
            return false;
        }
};

class lambertian : public material
{
    public:
        lambertian(const color& a) : albedo(a) {}

        bool scatter(const vec3& dirIn, const vec3& normal, color& attenuation, vec3& scattered) const override
        {
            vec3 scatteredDir = normal + randomUnitVector();

            if(scatteredDir.nearZero())
                scatteredDir = normal;

            scattered = scatteredDir;
            attenuation = albedo;
            return true;
        }

    private:
        color albedo;
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
