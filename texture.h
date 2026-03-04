#ifndef TEXTURE_H
#define TEXTURE_H

#include "color.h"
#include "rtw_stb_imge.h"

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

#endif