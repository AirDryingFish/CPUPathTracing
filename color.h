#ifndef COLOR_H
#define COLOR_H

#include "vec3.h"
#include "interval.h"

using color = vec3;

inline double linear2gamma(double linear_value) {
    if (linear_value > 0) {
        return std::sqrt(linear_value);
    }
    return 0;
}

// color ~ [0.0, 1.0]
void write_color(std::ostream& out, const color& pixel_color){
    double r = pixel_color.x();
    double g = pixel_color.y();
    double b = pixel_color.z();

    r = linear2gamma(r);
    g = linear2gamma(g);
    b = linear2gamma(b);

    static const interval intensity(0.000, 0.999);

    int rbyte = int(256 * intensity.clamp(r));
    int gbyte = int(256 * intensity.clamp(g));
    int bbyte = int(256 * intensity.clamp(b));
    out << rbyte << ' ' << gbyte << ' ' << bbyte << '\n';
}

#endif