#include <iostream>
#include <cmath>
#ifndef VEC3_H
#define VEC3_H

// Basic 3D vector class
class vec3 {
public:
    double e[3];
    vec3(): e{0, 0, 0} {}
    vec3(double e0, double e1, double e2): e{e0, e1, e2} {}
    double x() const { return e[0]; }
    double y() const { return e[1]; }
    double z() const { return e[2]; }

    vec3 operator-() const { return vec3(-e[0], -e[1], -e[2]); }
    double operator[](int i) const {return e[i];}
    double& operator[](int i) {return e[i];}

    vec3& operator+=(const vec3 & other){
        e[0] += other.e[0];
        e[1] += other.e[1];
        e[2] += other.e[2];
        return *this;
    }

    vec3& operator*=(const double t){
        e[0] *= t;
        e[1] *= t;
        e[2] *= t;
        return *this;
    }

    vec3& operator/=(const double t){
        *this *= 1 / t;
        return *this;
    }

    double length_squared() const {
        return e[0]*e[0] + e[1]*e[1] + e[2]*e[2];
    }

    double length() const {
        return std::sqrt(length_squared());
    }
};

// Make an alias for vec3 as point3
using point3 = vec3;

inline std::ostream& operator<<(std::ostream& out, const vec3& v){
    return out << "[vec3 Info] " << v.e[0] << ' ' << v.e[1] << ' ' << v.e[2];
}

inline vec3 operator+(const vec3& u, const vec3& v){
    return vec3(u.e[0] + v.e[0], u.e[1] + v.e[1], u.e[2] + v.e[2]);
}

inline vec3 operator-(const vec3& u, const vec3& v){
    return vec3(u.e[0] - v.e[0], u.e[1] - v.e[1], u.e[2] - v.e[2]);
}

inline vec3 operator*(const vec3& u, const vec3& v){
    return vec3(u.e[0] * v.e[0], u.e[1] * v.e[1], u.e[2] * v.e[2]);
}
inline vec3 operator*(const vec3& u, double t){
    return vec3(u.e[0] * t, u.e[1] * t, u.e[2] * t);
}
inline vec3 operator*(double t, const vec3& u){
    return u * t;
}

inline vec3 operator/(const vec3& u, double t){
    return u * (1 / t);
}

inline double dot(const vec3& u, const vec3& v){
    return u.e[0] * v.e[0]
         + u.e[1] * v.e[1]
         + u.e[2] * v.e[2];
}

inline vec3 cross(const vec3& u, const vec3& v){
    return vec3(
        u.e[1] * v.e[2] - u.e[2] * v.e[1],
        u.e[2] * v.e[0] - u.e[0] * v.e[2],
        u.e[0] * v.e[1] - u.e[1] * v.e[0]
    );
}

inline vec3 normalize(const vec3& v) {
    return v / v.length();
}


#endif