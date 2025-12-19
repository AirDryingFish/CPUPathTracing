#ifndef BVH_H
#define BVH_H


#include "hittable_list.h"

#include <algorithm>


class bvh_node : public hittable {
private:
    shared_ptr<hittable> left;
    shared_ptr<hittable> right;
    aabb bbox;

    // 根据物体的AABB在某个axis上的最小值来排序
    static bool box_compare(
        const shared_ptr<hittable> a, const shared_ptr<hittable> b, int axis_index
    )
    {
        auto a_axis_interval = a->bounding_box().axis_interval(axis_index);
        auto b_axis_interval = b->bounding_box().axis_interval(axis_index);
        return a_axis_interval.min < b_axis_interval.min;
    }

    static bool box_x_compare(const shared_ptr<hittable> a, const shared_ptr<hittable> b) {
        return box_compare(a, b, 0);
    }
    
    static bool box_y_compare(const shared_ptr<hittable> a, const shared_ptr<hittable> b) {
        return box_compare(a, b, 1);
    }
    
    static bool box_z_compare(const shared_ptr<hittable> a, const shared_ptr<hittable> b) {
        return box_compare(a, b, 2);
    }


public:
    bvh_node(hittable_list obj_list) : bvh_node(obj_list.objects, 0, obj_list.objects.size()) {

    }

    // **迭代创建树节点**
    bvh_node(std::vector<shared_ptr<hittable>>& obj_list, size_t start, size_t end) {
        // int axis = random_int(0, 2);
        bbox = aabb::empty;
        for (size_t obj_index = start; obj_index < end; obj_index++) {
            bbox = aabb(bbox, obj_list[obj_index]->bounding_box());
        }
        auto axis = bbox.longest_axiz();
        auto comparator = (axis == 0) ? box_x_compare :
                          (axis == 1) ? box_y_compare :
                                        box_z_compare;
        size_t object_span = end - start;
        // 如果只剩一个物体，左右节点都赋值为它
        if (object_span == 1) {
            left = right = obj_list[start];
        }

        else if (object_span == 2) {
            left = obj_list[start];
            right = obj_list[start + 1];
        }

        else {
            std::sort(std::begin(obj_list) + start, std::begin(obj_list) + end, comparator);
            
            auto mid = start + object_span / 2;
            left = make_shared<bvh_node>(obj_list, start, mid);
            right = make_shared<bvh_node>(obj_list, mid, end);
        }

        bbox = aabb(left->bounding_box(), right->bounding_box());

    }

    virtual bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
        if (!bbox.hit(r, ray_t)) {
            return false;
        }
        bool hit_left = left->hit(r, ray_t, rec);
        // 如果左子树命中了，右子树只考虑更近的情况。否则命中结果可能会被远处的覆盖
        bool hit_right = right->hit(r, interval(ray_t.min, hit_left ? rec.t : ray_t.max), rec);
        return hit_left || hit_right;
    }

    aabb bounding_box() const {
        return bbox;
    };
};



#endif