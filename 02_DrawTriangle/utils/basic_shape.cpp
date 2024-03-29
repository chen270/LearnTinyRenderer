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

// 利用重心坐标判断点是否在三角形内部
static Vec3f barycentric(Vec2i* pts, Vec2i P)
{
    Vec3i a = Vec3i(pts[1].x - pts[0].x, pts[2].x - pts[0].x, pts[0].x - P.x);
    Vec3i b = Vec3i(pts[1].y - pts[0].y, pts[2].y - pts[0].y, pts[0].y - P.y);
    Vec3i u = a ^ b; // u 向量和 x y 向量的点积为 0，所以 x y 向量叉乘可以得到 u 向量

    if (std::abs(u.z) <= 0.01)
        return Vec3f(-1, 1, 1); // 三点共线时，此时返回(-1,1,1)

    float xU = u.x / (float)u.z;
    float xV = u.y / (float)u.z;
    float xW = (u.z - u.x - u.y) / (float)u.z;
    return Vec3f(xW, xU, xV);
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


    Vec2i pts[3] = { t0,t1,t2 };
    for (int x = x_min; x <= x_max; ++x)
    {
        for (int y = y_min; y <= y_max; ++y)
        {
            Vec3f bc_screen = barycentric(pts, { x,y });

            // 此点在三角形内（认为边也是三角形的一部分）
            if (bc_screen.x >= 0 && bc_screen.x <= 1 && bc_screen.y >= 0 && bc_screen.y <= 1 && bc_screen.z >= 0 && bc_screen.z <= 1)
                image.set(x, y, color);
        }
    }
}
