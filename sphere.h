#ifndef SPHERE_H
#define SPHERE_H

#include <cmath>

#include "hittable.h"
#include "vec3.h"

class sphere: public hittable {
public:
    sphere(const point3& center, double radius, shared_ptr<material> mat) : 
        center(center), radius(std::fmax(0, radius)), mat(mat) {

    }

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override{
        vec3 Q_to_C = center - r.origin();
        auto a = r.direction().length_squared();
        auto h = dot(r.direction(), Q_to_C);
        auto c = dot(Q_to_C, Q_to_C) - radius * radius;
        auto determine = h * h - a * c;
        if (determine < 0) {
            return false;
        }
        auto root = (h - std::sqrt(determine)) / a;
        if (!ray_t.surrounds(root)) {
            root = (h + std::sqrt(determine)) / a;
            if (!ray_t.surrounds(root)) {
                return false;
            } 
        }
        rec.p = r.at(root);
        rec.t = root;
        rec.mat = mat;
        vec3 outward_normal = (rec.p - center) / radius;

        rec.set_face_normal(r, outward_normal);
        return true;
    }

private:
    point3 center;
    double radius;
    shared_ptr<material> mat;
};

#endif