#ifndef AABB_H
#define AABB_H

#include "interval.h"
class aabb
{
public:
    interval x, y, z;
    aabb() {};
    aabb(const interval& x, const interval& y, const interval& z) : x(x), y(y), z(z) {}
    aabb(const point3& a, const point3& b) {
        x = (a.x() < b.x()) ? interval(a.x(), b.x()) : interval(b.x(), a.x());
        y = (a.y() < b.y()) ? interval(a.y(), b.y()) : interval(b.y(), a.y());
        z = (a.z() < b.z()) ? interval(a.z(), b.z()) : interval(b.z(), a.z()); 
    }

    aabb(aabb const& box1, aabb const& box2) {
        x = interval(box1.x, box2.x);
        y = interval(box1.y, box2.y);
        z = interval(box1.z, box2.z);
    }

    const interval& axis_interval(int n) const {
        if (n == 0) return x;
        if (n == 1) return y;
        return z;
    }

    bool hit(const ray& r, interval ray_t) const {
        const point3& ray_origin = r.origin();
        const vec3& ray_direction = r.direction();
        for (int axis = 0; axis < 3; axis++) {
            const interval& ax = axis_interval(axis);
            const double adinv = 1.0 / ray_direction[axis];
            double t0 = (ax.min - ray_origin[axis]) * adinv;
            double t1 = (ax.max - ray_origin[axis]) * adinv;
            if (t0 < t1) // t0是左端点
            {
                if (t0 > ray_t.min) { // 左端点最大值
                    ray_t.min = t0;
                }
                if (t1 < ray_t.max) { // 右端点最小值
                    ray_t.max = t1;
                }
            }

            else { // t1是左端点
                if (t1 > ray_t.min) { // 左端点最大值
                    ray_t.min = t1;
                }
                if (t0 < ray_t.max) { // 右端点最小值
                    ray_t.max = t0;
                }
            }

            if (ray_t.min >= ray_t.max) {
                return false;
            }
        }
        return true;
    }

    int longest_axiz() const {
        if (x.size() > y.size()) {
            return x.size() > z.size() ? 0 : 2;
        }
        else {
            return y.size() > z.size() ? 1 : 2;
        }
    }
    static const aabb empty, universe;
};

const aabb aabb::empty = aabb(interval::empty, interval::empty, interval::empty);
const aabb aabb::universe = aabb(interval::universe, interval::universe, interval::universe);

#endif