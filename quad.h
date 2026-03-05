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
        if (a + b > 1)
        {
            return false;
        }

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

#endif