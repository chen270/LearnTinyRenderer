#ifndef __BASIC_SHAPE_H__
#define __BASIC_SHAPE_H__

#include "geometry.h"
#include "tgaimage.h"

namespace TinyColor
{
    const TGAColor red = TGAColor(255, 0, 0, 255);
    const TGAColor white = TGAColor(255, 255, 255, 255);
    const TGAColor green = TGAColor(0, 255, 0, 255);
    const TGAColor RandColor();
};


class Line
{
public:
    Line() = delete;
    Line(Vec2i t0, Vec2i t1, TGAImage &image, TGAColor color) { draw_line(t0.x, t0.y, t1.x, t1.y, image, color); };
    ~Line() = default;

private:
    void draw_line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color);
};

class Triangle
{
public:
    Triangle(/* args */) = delete;
    Triangle(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage &image, TGAColor color) { triangle_crossProduct(t0, t1, t2, image, color); };
    ~Triangle() = default;

private:
    void triangle_scan(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage &image, TGAColor color);
    void triangle_crossProduct(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage &image, TGAColor color);
    void triangle_barycenter(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage &image, TGAColor color);
};

#endif // __BASIC_SHAPE_H__