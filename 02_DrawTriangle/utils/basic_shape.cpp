#include "basic_shape.h"
#include <random>

namespace TinyColor
{
    const TGAColor RandColor()
    {
        static std::random_device rd;
        static std::mt19937 rng(rd());
        static std::uniform_int_distribution<int> dist(0, 255);
        return TGAColor(dist(rng), dist(rng), dist(rng), 255);
    }
}

void Line::draw_line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color)
{
    bool steep = false;
    if (std::abs(x0 - x1) < std::abs(y0 - y1))
    { 
        // 防止线过抖
        std::swap(x0, y0);
        std::swap(x1, y1);
        steep = true;
    }
    if (x0 > x1)
    { 
        // 线段的点绘制顺序问题
        std::swap(x0, x1);
        std::swap(y0, y1);
    }
    int dx = x1 - x0;
    int dy = y1 - y0;
    int derror2 = std::abs(dy) * 2;
    int error2 = 0;
    int y = y0;
    for (int x = x0; x <= x1; x++)
    {
        if (steep)
            image.set(y, x, color);
        else
            image.set(x, y, color);
        error2 += derror2;
        if (error2 > dx)
        {
            y += (y1 > y0 ? 1 : -1);
            error2 -= dx * 2;
        }
    }
}

void Triangle::triangle_scan(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage &image, TGAColor color)
{
    // 共线，三角形面积为0
    if ((t0.y == t1.y && t0.y == t2.y) || (t0.x == t1.x && t0.x == t2.x))
        return;

    // 根据y的大小对坐标进行排序
    // sort the vertices, t0, t1, t2 lower−to−upper (bubblesort yay!)
    if (t0.y > t1.y)
        std::swap(t0, t1);
    if (t0.y > t2.y)
        std::swap(t0, t2);
    if (t1.y > t2.y)
        std::swap(t1, t2);

    // 线框
    // line(t0, t1, image, color);
    // line(t1, t2, image, color);
    // line(t2, t0, image, color);

    float k02_x = (t2.x - t0.x) / (float)(t2.y - t0.y);
    float k01_x = (t1.x - t0.x) / (float)(t1.y - t0.y);
    float k12_x = (t2.x - t1.x) / (float)(t2.y - t1.y);
    for (int col = t0.y; col <= t2.y; ++col)
    {
        if (col <= t1.y)
        {
            // 下半部 t0.y - t1.y
            int x_min = (int)(k02_x * (col - t0.y) + t0.x);
            int x_max = (int)(k01_x * (col - t0.y) + t0.x);
            if (x_min > x_max)
                std::swap(x_min, x_max);
            for (int row = x_min; row <= x_max; ++row)
                image.set(row, col, color);
        }
        else
        {
            // 上半部 t2.y - t1.y
            int x_min = (int)(k02_x * (col - t0.y) + t0.x);
            int x_max = (int)(k12_x * (col - t1.y) + t1.x);
            if (x_min > x_max)
                std::swap(x_min, x_max);
            for (int row = x_min; row <= x_max; ++row)
                image.set(row, col, color);
        }
    }
}

static Vec3i crossProduct(Vec2i vec_a, Vec2i vec_b)
{
    Vec3i vecA = Vec3i(vec_a.x, vec_a.y, 0);
    Vec3i vecB = Vec3i(vec_b.x, vec_b.y, 0);
    return vecA ^ vecB;
}

void Triangle::triangle_crossProduct(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage &image, TGAColor color)
{
    // 共线，三角形面积为0
    if ((t0.y == t1.y && t0.y == t2.y) || (t0.x == t1.x && t0.x == t2.x))
        return;

    // 拿到 AABB 包围盒
    int x_min = std::min(std::min(t0.x, t1.x), t2.x);
    int y_min = std::min(std::min(t0.y, t1.y), t2.y);
    int x_max = std::max(std::max(t0.x, t1.x), t2.x);
    int y_max = std::max(std::max(t0.y, t1.y), t2.y);

    Vec3i a, b, c;
    Vec2i line01 = t1 - t0;
    Vec2i line12 = t2 - t1;
    Vec2i line20 = t0 - t2;
    for (int x = x_min; x <= x_max; ++x)
    {
        for (int y = y_min; y <= y_max; ++y)
        {
            Vec2i p(x, y);
            a = crossProduct(line01, p - t0);
            b = crossProduct(line12, p - t1);
            c = crossProduct(line20, p - t2);
            if ((a.z >= 0 && b.z >= 0 && c.z >= 0) || (a.z <= 0 && b.z <= 0 && c.z <= 0))
                image.set(x, y, color);
        }
    }
}

void Triangle::triangle_barycenter(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage &image, TGAColor color)
{
    // 共线，三角形面积为0
    if ((t0.y == t1.y && t0.y == t2.y) || (t0.x == t1.x && t0.x == t2.x))
        return;

    // 拿到 AABB 包围盒
    int x_min = std::min(std::min(t0.x, t1.x), t2.x);
    int y_min = std::min(std::min(t0.y, t1.y), t2.y);
    int x_max = std::max(std::max(t0.x, t1.x), t2.x);
    int y_max = std::max(std::max(t0.y, t1.y), t2.y);

    Vec3f a = Vec3f(t1.x - t0.x, t2.x - t0.x, 0);
    Vec3f b = Vec3f(t1.y - t0.y, t2.y - t0.y, 0);
    Vec3f BarCoord;
    for (int x = x_min; x <= x_max; ++x)
    {
        for (int y = y_min; y <= y_max; ++y)
        {
            a.z = t0.x - x;
            b.z = t0.y - y;
            BarCoord = a ^ b;
            BarCoord = BarCoord * (1.0f / BarCoord.z);
            float u = BarCoord.x;
            float v = BarCoord.y;
            if (u >= 0 && u < 1 && v >= 0 && v <= 1 && 1 - u - v >= 0 && 1 - u - v < 1)
                image.set(x, y, color);
        }
    }
}