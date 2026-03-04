#ifndef TEXTURE_H
#define TEXTURE_H

#include "color.h"
#include "rtw_stb_imge.h"
#include "perlin.h"

class texture
{
public:
    virtual ~texture() = default;

    virtual color value(double u, double v, const point3 &p) const = 0;
};

class solid_color : public texture
{
private:
    color albedo;

public:
    solid_color(const color &albedo) : albedo(albedo) {}

    solid_color(double red, double green, double blue) : solid_color(color(red, green, blue)) {}

    color value(double u, double v, const point3 &p) const override
    {
        return albedo;
    }
};

class checker_texture : public texture
{
private:
    double inv_scale;
    shared_ptr<texture> odd_tex;
    shared_ptr<texture> even_tex;

public:
    checker_texture(double scale, shared_ptr<texture> odd, shared_ptr<texture> even)
        : inv_scale(1.0 / scale), odd_tex(odd), even_tex(even) {}

    checker_texture(double scale, const color &odd_color, const color &even_color) : checker_texture(scale, make_shared<solid_color>(odd_color), make_shared<solid_color>(even_color)) {}

    color value(double u, double v, const point3 &p) const override
    {
        auto xInterger = int(std::floor(p.x() * inv_scale));
        auto yInterger = int(std::floor(p.y() * inv_scale));
        auto zInterger = int(std::floor(p.z() * inv_scale));
        if ((xInterger + yInterger + zInterger) % 2 == 0)
        {
            return even_tex->value(u, v, p);
        }
        else
        {
            return odd_tex->value(u, v, p);
        }
    }
};

class image_texture : public texture
{
public:
    image_texture(const char *filename) : image(filename) {}

    color value(double u, double v, const point3 &p) const override
    {
        if (image.height() <= 0)
        {
            return color(0, 1, 1);
        }

        u = interval(0, 1).clamp(u);
        v = 1.0 - interval(0, 1).clamp(v);

        auto i = int(u * image.width());
        auto j = int(v * image.height());
        auto pixel = image.pixel_data(i, j);
        auto color_scale = 1.0 / 255.0;
        return color(color_scale * pixel[0], color_scale * pixel[1], color_scale * pixel[2]);
    }

private:
    rtw_image image;
};

class noise_texture : public texture
{
public:
    noise_texture(float scale) : scale(scale) {}

    virtual color value(double u, double v, const point3 &p) const
    {
        // return color(1, 1, 1) * noise.turb(p, 10);

        // 1. scale * p.z()： 基本的正弦函数输入，用 p.z()（物体空间的 z 坐标）乘上一个缩放因子 scale，生成沿 z 方向的均匀条纹。
        // 2. noise.turb(p, 10)： 计算点 p 的湍流值（前面讨论过的多频率噪声），深度 10 表示叠加 10 层八度。
        // 3. 相位扰动： 把湍流乘以 10 并加到正弦的输入上 → 原本直线条纹的相位被不规则地偏移，条纹随空间起伏，看起来就像大理石中的纹理
        return color(0.5, 0.5, 0.5) * (1 + std::sin(scale * p.z() + 10 * noise.turb(p, 7)));
    }

private:
    perlin noise;
    double scale;
};

#endif