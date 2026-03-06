#ifndef QUAD_H
#define QUAD_H

#include "hittable.h"

class quad : public hittable
{
public:
    quad(const point3 &Q, const vec3 &u, const vec3 &v, shared_ptr<material> mat) : Q(Q), u(u), v(v), mat(mat)
    {

        auto n_t = cross(u, v);
        n = normalize(n_t);
        D = dot(Q, n);
        w = n_t / dot(n_t, n_t);

        set_bounding_box();
    }

    virtual void set_bounding_box()
    {
        auto bbox_diagonal1 = aabb(Q, Q + u + v);
        auto bbox_diagonal2 = aabb(Q + u, Q + v);
        bbox = aabb(bbox_diagonal1, bbox_diagonal2);
    }

    aabb bounding_box() const override
    {
        return bbox;
    }

    bool hit(const ray &r, interval ray_t, hit_record &rec) const override
    {
        auto denom = dot(n, r.direction());
        if (std::fabs(denom) < 1e-8)
        {
            return false;
        }

        auto t = (D - dot(n, r.origin())) / denom;
        if (!ray_t.contains(t))
        {
            return false;
        }

        auto intersection = r.at(t);
        vec3 planar_hitpt_vec = intersection - Q;
        auto alpha = dot(w, cross(planar_hitpt_vec, v));
        auto beta = dot(w, cross(u, planar_hitpt_vec));

        if (!is_interior(alpha, beta, rec))
        {
            return false;
        }

        rec.t = t;
        rec.p = intersection;
        rec.mat = mat;
        rec.set_face_normal(r, n);
        return true;
    }

    // 通过改变这个函数的返回条件，可以定义不同的2d primitive
    virtual bool is_interior(double a, double b, hit_record &rec) const
    {
        interval unit_interval = interval(0, 1);
        if (!unit_interval.contains(a) || !unit_interval.contains(b))
        {
            return false;
        }

        // triangle definition
        // if (a + b > 1)
        // {
        //     return false;
        // }

        rec.u = a;
        rec.v = b;
        return true;
    }

private:
    point3 Q;
    vec3 u, v;
    vec3 w;
    shared_ptr<material> mat;
    aabb bbox;
    vec3 n;
    double D;
};

inline shared_ptr<hittable_list> box(const point3 &a, const point3 &b, shared_ptr<material> mat)
{
    auto sides = make_shared<hittable_list>();
    auto min = point3(std::fmin(a.x(), b.x()), std::fmin(a.y(), b.y()), std::fmin(a.z(), b.z()));
    auto max = point3(std::fmax(a.x(), b.x()), std::fmax(a.y(), b.y()), std::fmax(a.z(), b.z()));

    auto dx = vec3(max.x() - min.x(), 0, 0);
    auto dy = vec3(0, max.y() - min.y(), 0);
    auto dz = vec3(0, 0, max.z() - min.z());

    sides->add(make_shared<quad>(point3(min.x(), min.y(), max.z()), dx, dy, mat));  // front
    sides->add(make_shared<quad>(point3(max.x(), min.y(), max.z()), -dz, dy, mat)); // right
    sides->add(make_shared<quad>(point3(max.x(), min.y(), min.z()), -dx, dy, mat)); // back
    sides->add(make_shared<quad>(point3(min.x(), min.y(), min.z()), dz, dy, mat));  // left
    sides->add(make_shared<quad>(point3(min.x(), max.y(), max.z()), dx, -dz, mat)); // top
    sides->add(make_shared<quad>(point3(min.x(), min.y(), min.z()), dx, dz, mat));  // bottom

    return sides;
}

#endif