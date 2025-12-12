#ifndef MATERIAL_H
#define MATERIAL_H

#include "hittable.h"
#include "color.h"

class material
{
public:
    virtual ~material() = default;

    virtual bool scatter(
        const ray& r_in,        // 入射光线
        const hit_record& rec,  // 光线与物体的交点信息
        color& attenuation,     // 衰减，材质吸收后的颜色
        ray& scattered          // 输出的散射后的新光线（反射或折射方向）
    ) const {
        return false;           // 光线成功散射->继续追踪下条射线；否则吸收光线，返回黑色
    }
};

class lambertian : public material {
public:
    lambertian(const color& albedo): albedo(albedo) {}
    virtual bool scatter(
        const ray& r_in,        // 入射光线
        const hit_record& rec,  // 光线与物体的交点信息
        color& attenuation,     // 衰减，材质吸收后的颜色
        ray& scattered          // 输出的散射后的新光线（反射或折射方向）
    ) const override {
        auto scatter_direction = rec.normal + random_unit_vector();

        while (scatter_direction.near_zero()) {
            scatter_direction = rec.normal + random_unit_vector();
        }

        scattered = ray(rec.p, scatter_direction);
        attenuation = albedo;
        return true;
    }
private:
    color albedo;
};

class metal : public material {
public:
    metal(const color& albedo, double fuzz): albedo(albedo), fuzz(fuzz < 1 ? fuzz : 1) {}
    virtual bool scatter(
        const ray& r_in,        // 入射光线
        const hit_record& rec,  // 光线与物体的交点信息
        color& attenuation,     // 衰减，材质吸收后的颜色
        ray& scattered          // 输出的散射后的新光线（反射或折射方向）
    ) const override {
        vec3 reflected = reflect(r_in.direction(), rec.normal);
        reflected = normalize(reflected) + fuzz * random_unit_vector();
        scattered = ray(rec.p, reflected);
        attenuation = albedo;
        return dot(reflected, rec.normal) > 0;
    }

private:
    color albedo;
    double fuzz;
};

class dielectric : public material {
public:
    dielectric(double refraction_index): refraction_index(refraction_index) {}
        virtual bool scatter(
        const ray& r_in,        // 入射光线
        const hit_record& rec,  // 光线与物体的交点信息
        color& attenuation,     // 衰减，材质吸收后的颜色
        ray& scattered          // 输出的散射后的新光线（反射或折射方向）
    ) const override {
        attenuation = color(1, 1, 1);
        double ri = rec.front_face ? (1.0 / refraction_index): refraction_index;
        vec3 r_in_normalized = normalize(r_in.direction());

        double cos_theta = fmin(dot(-r_in_normalized, rec.normal), 1.0);
        double sin_theta = std::sqrt(1.0 - cos_theta * cos_theta);
        bool cannot_refract = ri * sin_theta > 1.0;
        vec3 direction;
        if (cannot_refract || reflactance(cos_theta, ri) > random_double()) {
            direction = reflect(r_in_normalized, rec.normal);
        }
        else {
            direction = refract(r_in_normalized, rec.normal, ri);
        }
        scattered = ray(rec.p, direction);
        return true;
    }


private:
    double refraction_index;
    static double reflactance(double cosine, double refraction_index) {
        auto r0 = (1 - refraction_index) / (1 + refraction_index);
        r0 = r0 * r0;
        return r0 + (1 - r0) * std::pow((1 - cosine), 5);
    }
};


#endif