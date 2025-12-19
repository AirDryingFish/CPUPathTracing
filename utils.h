#ifndef UTILS
#define UTILS

#include <cmath>
#include <iostream>
#include <limits>
#include <memory>
#include <cstdlib>

// C++ Std Usings
using std::shared_ptr;
using std::make_shared;

// Constants
const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;

// Utility Functions
inline double degree_to_radians(double degree) {
    return degree / 180.0 * pi;
}


inline double random_double() {
    return std::rand() / (RAND_MAX + 1.0);
}

inline double random_double(double min, double max) {
    return random_double() * (max - min) + min;
}

inline int random_int(int min, int max) {
    return int(random_double(min, max + 1));
}


// Common Headers
#include "color.h"
#include "ray.h"
#include "vec3.h"
#include "interval.h"

#endif