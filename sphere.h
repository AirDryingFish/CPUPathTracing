#ifndef SPHERE_H
#define SPHERE_H

#include <cmath>

#include "hittable.h"
#include "vec3.h"

class sphere: public hittable {
public:
    // stationary sphere
    sphere(const point3& center, double radius, shared_ptr<material> mat) : 
        center(center, vec3(0, 0, 0)), radius(std::fmax(0, radius)), mat(mat) {
    }

    // moving sphere
    sphere(const point3& center1, const point3& center2, double radius, shared_ptr<material> mat) :
        center(center1, center2 - center1), radius(std::fmax(0, radius)), mat(mat) {
        // to be implemented
    }

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override{
        point3 current_center = center.at(r.time());
        vec3 Q_to_C = current_center - r.origin();
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
        vec3 outward_normal = (rec.p - current_center) / radius;

        rec.set_face_normal(r, outward_normal);
        return true;
    }

private:
    ray center;
    double radius;
    shared_ptr<material> mat;
};

#endif