#ifndef MATERIAL_H
#define MATERIAL_H

#include "hittable.h"
#include "color.h"
#include "aabb.h"
#include "texture.h"

class material
{
public:
    virtual ~material() = default;

    // emitting material emits light, non-emitting material returns black
    virtual color emitted(double u, double v, const point3 &p) const
    {
        return color(0, 0, 0);
    }

    virtual bool scatter(
        const ray &r_in,       // 入射光线
        const hit_record &rec, // 光线与物体的交点信息
        color &attenuation,    // 衰减，材质吸收后的颜色
        ray &scattered         // 输出的散射后的新光线（反射或折射方向）
    ) const
    {
        return false; // 光线成功散射->继续追踪下条射线；否则吸收光线，返回黑色
    }
};

class lambertian : public material
{
private:
    shared_ptr<texture> tex;

public:
    // lambertian(const color& albedo): albedo(albedo) {}
    lambertian(const color &albedo) : tex(make_shared<solid_color>(albedo)) {}
    lambertian(shared_ptr<texture> tex) : tex(tex) {}
    virtual bool scatter(
        const ray &r_in,       // 入射光线
        const hit_record &rec, // 光线与物体的交点信息
        color &attenuation,    // 衰减，材质吸收后的颜色
        ray &scattered         // 输出的散射后的新光线（反射或折射方向）
    ) const override
    {
        auto scatter_direction = rec.normal + random_unit_vector();

        while (scatter_direction.near_zero())
        {
            scatter_direction = rec.normal + random_unit_vector();
        }

        scattered = ray(rec.p, scatter_direction, r_in.time());
        attenuation = tex->value(rec.u, rec.v, rec.p);
        return true;
    }
};

class metal : public material
{
public:
    metal(const color &albedo, double fuzz) : albedo(albedo), fuzz(fuzz < 1 ? fuzz : 1) {}
    virtual bool scatter(
        const ray &r_in,       // 入射光线
        const hit_record &rec, // 光线与物体的交点信息
        color &attenuation,    // 衰减，材质吸收后的颜色
        ray &scattered         // 输出的散射后的新光线（反射或折射方向）
    ) const override
    {
        vec3 reflected = reflect(r_in.direction(), rec.normal);
        reflected = normalize(reflected) + fuzz * random_unit_vector();
        scattered = ray(rec.p, reflected, r_in.time());
        attenuation = albedo;
        return dot(reflected, rec.normal) > 0;
    }

private:
    color albedo;
    double fuzz;
};

class dielectric : public material
{
public:
    dielectric(double refraction_index) : refraction_index(refraction_index) {}
    virtual bool scatter(
        const ray &r_in,       // 入射光线
        const hit_record &rec, // 光线与物体的交点信息
        color &attenuation,    // 衰减，材质吸收后的颜色
        ray &scattered         // 输出的散射后的新光线（反射或折射方向）
    ) const override
    {
        attenuation = color(1, 1, 1);
        double ri = rec.front_face ? (1.0 / refraction_index) : refraction_index;
        vec3 r_in_normalized = normalize(r_in.direction());

        double cos_theta = fmin(dot(-r_in_normalized, rec.normal), 1.0);
        double sin_theta = std::sqrt(1.0 - cos_theta * cos_theta);
        bool cannot_refract = ri * sin_theta > 1.0;
        vec3 direction;
        if (cannot_refract || reflactance(cos_theta, ri) > random_double())
        {
            direction = reflect(r_in_normalized, rec.normal);
        }
        else
        {
            direction = refract(r_in_normalized, rec.normal, ri);
        }
        scattered = ray(rec.p, direction, r_in.time());
        return true;
    }

private:
    double refraction_index;
    static double reflactance(double cosine, double refraction_index)
    {
        auto r0 = (1 - refraction_index) / (1 + refraction_index);
        r0 = r0 * r0;
        return r0 + (1 - r0) * std::pow((1 - cosine), 5);
    }
};

class diffuse_light : public material
{
public:
    diffuse_light(shared_ptr<texture> tex) : tex(tex) {}
    diffuse_light(const color &emit_color) : tex(make_shared<solid_color>(emit_color)) {}

    color emitted(double u, double v, const point3 &p) const override
    {
        return tex->value(u, v, p);
    }

private:
    shared_ptr<texture> tex;
};

// 各向同性散射：光线在球面上均匀随机散射（各向同性）。
class isotropic : public material
{
public:
    isotropic(const color &albedo) : tex(make_shared<solid_color>(albedo)) {}
    isotropic(shared_ptr<texture> tex) : tex(tex) {}

    bool scatter(
        const ray &r_in,       // 入射光线
        const hit_record &rec, // 光线与物体的交点信息
        color &attenuation,    // 衰减，材质吸收后的颜色
        ray &scattered         // 输出的散射后的新光线（反射或折射方向）
    ) const override
    {
        scattered = ray(rec.p, random_unit_vector(), r_in.time());
        attenuation = tex->value(rec.u, rec.v, rec.p);
        return true;
    }

private:
    shared_ptr<texture> tex;
};

#endif