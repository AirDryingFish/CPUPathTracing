#ifndef HITTABLE
#define HITTABLE

#include "ray.h"
#include "aabb.h"

class material;

class hit_record
{
public:
    point3 p;
    vec3 normal;
    shared_ptr<material> mat;
    double t; // ray walk distance to the hit point
    double u;
    double v;
    bool front_face;

    // outward_normal is assumed to have unit length.
    void set_face_normal(const ray &r, const vec3 &outward_normal)
    {
        front_face = dot(r.direction(), outward_normal) < 0;
        normal = front_face ? outward_normal : -outward_normal;
    }
};

class hittable
{
public:
    virtual ~hittable() = default;
    virtual bool hit(const ray &r, interval ray_t, hit_record &rec) const = 0;

    virtual aabb bounding_box() const = 0;
};

class translate : public hittable
{
public:
    translate(shared_ptr<hittable> object, const vec3 &offset) : object(object), offset(offset)
    {
        bbox = object->bounding_box() + offset;
    }

    bool hit(const ray &r, interval ray_t, hit_record &rec) const override
    {
        // 1. 将射线平移到物体的局部坐标系中
        // 2. 在局部坐标系中进行相交测试，如果不相交则返回false
        // 3. 如果相交了，将交点变换为世界坐标系，并更新hit_record中的交点和法线等信息
        ray moved_r(r.origin() - offset, r.direction(), r.time());
        if (!object->hit(moved_r, ray_t, rec))
        {
            return false;
        }
        rec.p += offset; // 将交点平移回原位置
        return true;
    }

    aabb bounding_box() const override
    {
        return bbox;
    }

private:
    shared_ptr<hittable> object;
    vec3 offset;
    aabb bbox;
};

class rotate_y : public hittable
{
public:

    rotate_y(shared_ptr<hittable> object, double angle) : object(object)
    {
        auto radians = degree_to_radians(angle);
        sin_theta = std::sin(radians);
        cos_theta = std::cos(radians);
        bbox = object->bounding_box();

        point3 min(infinity, infinity, infinity);
        point3 max(-infinity, -infinity, -infinity);

        for (int i = 0; i < 2; i++)
        {
            for (int j = 0; j < 2; j++)
            {
                for (int k = 0; k < 2; k++)
                {
                    auto x = i * bbox.x.max + (1 - i) * bbox.x.min;
                    auto y = j * bbox.y.max + (1 - j) * bbox.y.min;
                    auto z = k * bbox.z.max + (1 - k) * bbox.z.min;
                    
                    auto newx = cos_theta * x + sin_theta * z;
                    auto newz = -sin_theta * x + cos_theta * z;

                    vec3 tester(newx, y, newz);
                    for (int c = 0; c < 3; c++)
                    {
                        min[c] = std::fmin(min[c], tester[c]);
                        max[c] = std::fmax(max[c], tester[c]);
                    }
                }
            }
        }

        bbox = aabb(min, max);


    }

    bool hit(const ray &r, interval ray_t, hit_record &rec) const override
    {
        // World -> Object: rotate by -theta
        auto origin = point3(cos_theta * r.origin().x() - sin_theta * r.origin().z(),
                             r.origin().y(),
                             sin_theta * r.origin().x() + cos_theta * r.origin().z());

        auto direction = vec3(cos_theta * r.direction().x() - sin_theta * r.direction().z(),
                              r.direction().y(),
                              sin_theta * r.direction().x() + cos_theta * r.direction().z());

        ray rotated_r(origin, direction, r.time());

        // intersection judge in Object space
        if (!object->hit(rotated_r, ray_t, rec))
        {
            return false;
        }

        // Object -> World: rotate by theta, transform intersection info back to world space
        rec.p = point3(cos_theta * rec.p.x() + sin_theta * rec.p.z(),
                       rec.p.y(),
                       -sin_theta * rec.p.x() + cos_theta * rec.p.z());
        rec.normal = vec3(cos_theta * rec.normal.x() + sin_theta * rec.normal.z(),
                          rec.normal.y(),
                          -sin_theta * rec.normal.x() + cos_theta * rec.normal.z());
        return true;
    }

    aabb bounding_box() const override
    {
        return bbox;
    }

private:
    shared_ptr<hittable> object;
    double sin_theta;
    double cos_theta;
    aabb bbox;
};

#endif