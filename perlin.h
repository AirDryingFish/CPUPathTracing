#ifndef PERLIN_H
#define PERLIN_H

#include "utils.h"

class perlin
{
public:
    perlin()
    {
        for (int i = 0; i < point_count; i++)
        {
            // randfloat[i] = random_double();
            // Four Attempt: 2.在每个格点存一个随机单位向量，而不是一个数值
            randvec[i] = normalize(vec3::random(-1, 1));
        }
        perlin_generate_perm(perm_x);
        perlin_generate_perm(perm_y);
        perlin_generate_perm(perm_z);
    }

    // First Attempt: grid noise
    // double noise(const point3 &p) const
    // {
    //     auto i = int(10 * p.x()) & 255;
    //     auto j = int(10 * p.y()) & 255;
    //     auto k = int(10 * p.z()) & 255;
    //     return randfloat[perm_x[i] ^ perm_y[j] ^ perm_z[k]];
    // }

    // Second Attempt: Smoothing out the Result
    double noise(const point3 &p) const
    {

        // uvw：小数部分，点在那个 1×1×1 单位立方内的位置
        auto u = p.x() - std::floor(p.x());
        auto v = p.y() - std::floor(p.y());
        auto w = p.z() - std::floor(p.z());

        // Third Attempt: Improvement with Hermitian Smoothing
        // u = u * u * (3 - 2 * u);
        // v = v * v * (3 - 2 * v);
        // w = w * w * (3 - 2 * w);

        // ijk: 格点整数坐标
        auto i = int(std::floor(p.x()));
        auto j = int(std::floor(p.y()));
        auto k = int(std::floor(p.z()));

        // double c[2][2][2];
        vec3 c[2][2][2];

        // Four Attempt: 1. 每个格点只存一个随机浮点值，结果是噪声的极大/极小值总是出现在正好落在整数格点的位置，导致图案看起来有棱有角、稍显“方块感”
        for (int di = 0; di < 2; di++)
        {
            for (int dj = 0; dj < 2; dj++)
            {
                for (int dk = 0; dk < 2; dk++)
                {
                    c[di][dj][dk] = randvec[perm_x[(i + di) & 255] ^
                                            perm_y[(j + dj) & 255] ^
                                            perm_z[(k + dk) & 255]];
                }
            }
        }
        // return trilinear_interp(c, u, v, w);
        return perlin_interp(c, u, v, w);
    }
    double turb(const point3 &p, int depth) const
    {
        auto accum = 0.0f;
        auto temp_p = p;    // 当前采样位置
        auto weight = 1.0f; // 当前八度的振幅
        for (int i = 0; i < depth; i++)
        {
            accum += weight * noise(temp_p); // 累加加权噪声
            weight *= 0.5;                   // 每一层振幅减半
            temp_p *= 2;                     // 频率加倍
        }
        return std::fabs(accum); // 取绝对值
    }

private:
    static const int point_count = 256;
    double randfloat[point_count];
    vec3 randvec[point_count];
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

    static double trilinear_interp(double c[2][2][2], double u, double v, double w)
    {
        auto accum = 0.0;
        // 三重循环遍历立方体的 8 个角
        for (int i = 0; i < 2; i++)
        {
            for (int j = 0; j < 2; j++)
            {
                for (int k = 0; k < 2; k++)
                {
                    accum += (i * u + (1 - i) * (1 - u)) *
                             (j * v + (1 - j) * (1 - v)) *
                             (k * w + (1 - k) * (1 - w)) * c[i][j][k];
                }
            }
        }
        return accum;
    }

    static double perlin_interp(const vec3 c[2][2][2], double u, double v, double w)
    {
        u = u * u * (3 - 2 * u);
        v = v * v * (3 - 2 * v);
        w = w * w * (3 - 2 * w);

        auto accum = 0.0;
        // 三重循环遍历立方体的 8 个角
        for (int i = 0; i < 2; i++)
        {
            for (int j = 0; j < 2; j++)
            {
                for (int k = 0; k < 2; k++)
                {
                    // 计算噪声时，对每个格点用它的向量与从格点到采样点的偏移向量做 点积
                    // 这个点积会随着采样点在单元格内的精确位置连续变化，因此噪声的最大最小值不再固定在格点上，边界 artefact 被消除，结果更加平滑自然。
                    vec3 weight_v(u - i, v - j, w - k);
                    accum += (i * u + (1 - i) * (1 - u)) *
                             (j * v + (1 - j) * (1 - v)) *
                             (k * w + (1 - k) * (1 - w)) * dot(c[i][j][k], weight_v);
                }
            }
        }
        return accum;
    }
};

#endif