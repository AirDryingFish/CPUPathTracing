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



#endif