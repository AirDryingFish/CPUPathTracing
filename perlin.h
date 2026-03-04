#ifndef PERLIN_H
#define PERLIN_H

#include "utils.h"

class perlin
{
public:
    perlin()
    {
        for (int i = 0; i <)
    }

private:
    static const int point_count = 256;
    double randfloat[point_count];
    int perm_x[point_count];
    int perm_y[point_count];
    int perm_z[point_count];
    static void perlin_generate_perm(int *p)
    {
        for (int i = 0; i < point_count; i++)
        {
            p[i] = i;
        }
        permute(p, point_count);
    }

    static void permute(int *p, int n)
    {
        for (int i = n - 1; i > 0; i--)
        {
            int target = random_int(0, i);
            int tmp = p[i];
            p[i] = p[target];
            p[target] = tmp;
        }
    }
};

#endif