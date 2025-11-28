#ifndef UTILS
#define UTILS

#include <cmath>
#include <iostream>
#include <limits>
#include <memory>

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

// Common Headers
#include "color.h"
#include "ray.h"
#include "vec3.h"
#include "interval.h"

#endif