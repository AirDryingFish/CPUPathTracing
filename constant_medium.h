#ifndef CONSTANT_MEDIUM_H
#define CONSTANT_MEDIUM_H

#include "hittable.h"
#include "material.h"
#include "texture.h"

class constant_medium : public hittable
{
public:
    constant_medium(shared_ptr<hittable> boundary, double density, shared_ptr<texture> tex)
        : boundary(boundary), neg_inv_density(-1.0 / density), phase_function(make_shared<isotropic>(tex))
    {
    }

    constant_medium(shared_ptr<hittable> boundary, double density, const color &albedo)
        : boundary(boundary), neg_inv_density(-1.0 / density), phase_function(make_shared<isotropic>(albedo))
    {
    }

    bool hit(const ray &r, interval ray_t, hit_record &rec) const override
    {
        hit_record rec1; // 光线进入边界的点
        hit_record rec2; // 光线离开边界的点

        if (!boundary->hit(r, interval::universe, rec1)) // Find first intersection within the infinite boundary
        {
            return false;
        }

        if (!boundary->hit(r, interval(rec1.t + 0.0001, infinity), rec2)) // Not find second intersection after the first one
        {
            return false;
        }

        // clip intersection times to the ray segment of interest
        // rec1.t is the entry time; if it's before our ray_t.min (e.g. the
        // ray started inside the volume or earlier than the allowed interval),
        // move it forward to the start of the valid segment.
        if (rec1.t < ray_t.min)
        {
            rec1.t = ray_t.min;
        }
        // rec2.t is the exit time; if it lies beyond ray_t.max (e.g. the volume
        // extends past an earlier hit or the ray end), clamp it back.
        // 当光线在进入体积后不久就与别的物体相交（ray_t.max）或者光线本身已经“到头”了，那么离开点可能在这个交点之后；为了避免把后面的那部分算进体积内部，必须把 rec2.t 截掉
        if (rec2.t > ray_t.max)
        {
            rec2.t = ray_t.max;
        }
        // after clipping we may have no valid segment at all (entry happens at or
        // after exit) – then the ray does not actually pass through the volume
        // within the permitted interval.
        if (rec1.t >= rec2.t)
        {
            return false;
        }
        // if the adjusted entry time is negative, the ray origin lies inside the
        // medium; start counting distance from t=0 rather than a negative value.
        if (rec1.t < 0)
        {
            rec1.t = 0;
        }

        auto ray_length = r.direction().length();                       // unit ray length along the ray direction
        auto distance_inside_boundary = (rec2.t - rec1.t) * ray_length; // ray segment length inside the boundary
        // according to fomula P(t) = 1 - exp(-density * t), the distance to the next scattering event is exponentially distributed with mean 1/density
        auto hit_distance = neg_inv_density * std::log(random_double());

        if (hit_distance > distance_inside_boundary) // No scattering happens within the boundary
        {
            return false;
        }

        rec.t = rec1.t + hit_distance / ray_length; // ray parameter t for the scattering event
        rec.p = r.at(rec.t);
        rec.normal = vec3(1, 0, 0);
        rec.front_face = true; // arbitrary
        rec.mat = phase_function;

        return true;
    }

    aabb bounding_box() const override
    {
        return boundary->bounding_box();
    }

private:
    shared_ptr<hittable> boundary;
    double neg_inv_density;
    shared_ptr<material> phase_function;
};

#endif