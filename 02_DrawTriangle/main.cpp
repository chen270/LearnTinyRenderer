#include "tgaimage.h"
#include "model.h"
#include "basic_shape.h"

#ifndef DIR_PATH
#error SourceDir path not define!
#else
#define S_PATH(str) DIR_PATH##str
#define OUT_PATH(str) DIR_PATH##"./bin/"##str
#endif
void triangle(Vec2i* pts, TGAImage& image, TGAColor color);

void myDrawModel()
{
    const int width = 200;
    const int height = 200;
    TGAImage image(width, height, TGAImage::RGB);

    Model* model = new Model(S_PATH("./obj/african_head.obj"));
    Vec3f lightDir = Vec3f(0, 0, -1);
    for (int i = 0; i < model->nfaces(); ++i) {
        std::vector<int> face = model->face(i);
        Vec2i aPoint[3];
        Vec3f world_coords[3];   //新加入一个数组用于存放三个顶点的世界坐标
        for (int j = 0; j < 3; j++) {
            Vec3f v0 = model->vert(face[j]);
            int x0 = (v0.x + 1.) * width / 2.;
            int y0 = (v0.y + 1.) * height / 2.;
            aPoint[j] = { x0, y0 }; // 简单的视口变换
            world_coords[j] = v0;
        }

        //这里三角形ABC 满足一定的顺序，即 AC x AB
        Vec3f n = ((world_coords[2] - world_coords[0]) ^ (world_coords[1] - world_coords[0])).normalize();
        float intensity = n * lightDir; //光照强度=法向量*光照方向   即法向量和光照方向重合时，亮度最高
        if (intensity > 0)
        {
            //Triangle(aPoint[0], aPoint[1], aPoint[2], image, TGAColor(intensity * 255, intensity * 255, intensity * 255, 255));
            Triangle(aPoint, image, TGAColor(intensity * 255, intensity * 255, intensity * 255, 255));
        }
    }


    //image.flip_vertically();
    image.write_tga_file(OUT_PATH("lesson00.tga"));
    delete model;
}

int main2(int argc, char** argv) {
    const int width = 200;
    const int height = 200;

    Vec2i t0[3] = { Vec2i(10, 10),   Vec2i(90, 90),  Vec2i(50, 130) };
    Vec2i t1[3] = { Vec2i(180, 50),  Vec2i(150, 1),   Vec2i(70, 180) };
    Vec2i t2[3] = { Vec2i(180, 150), Vec2i(120, 160), Vec2i(130, 180) };


    TGAImage image(width, height, TGAImage::RGB);
    //image.set(52, 41, red);
    //Line(10, 10, 50, 50, image, red);
    //Line(13, 20, 80, 40, image, white);
    //Line(20, 13, 40, 80, image, red);
    //Line(80, 40, 13, 20, image, red);
    //Triangle(t0[0], t0[1], t0[2], image, green);
    //Triangle(t1[0], t1[1], t1[2], image, white);
    //Triangle(t2[0], t2[1], t2[2], image, red);

    Model* model = new Model(S_PATH("./obj/african_head.obj"));
    Vec3f lightPos = Vec3f(0, 0, 1);
    for (int i = 0; i < model->nfaces(); ++i) {
        std::vector<int> face = model->face(i);
        Vec2i aPoint[3];
        Vec3f world_coords[3];   //新加入一个数组用于存放三个顶点的世界坐标
        for (int j = 0; j < 3; j++) {
            Vec3f v0 = model->vert(face[j]);
            int x0 = (v0.x + 1.) * width / 2.;
            int y0 = (v0.y + 1.) * height / 2.;
            aPoint[j] = { x0, y0 };
            world_coords[j] = v0;
        }

        Vec3f n = ((world_coords[1] - world_coords[0]) ^ (world_coords[2] - world_coords[0])).normalize();
        Vec3f dir = (lightPos - Vec3f(world_coords[0].x, world_coords[0].y, 0)).normalize();
        float intensity = n * dir;
        if (intensity > 0)
            Triangle(aPoint[0], aPoint[1], aPoint[2], image, TGAColor(intensity * 255, intensity * 255, intensity * 255, 255));
    }


    //image.flip_vertically();
    image.write_tga_file(OUT_PATH("lesson00.tga"));
    return 0;
}


// 利用重心坐标判断点是否在三角形内部
Vec3f barycentric(Vec2i* pts, Vec2i P) {
    Vec3i x(pts[1].x - pts[0].x, pts[2].x - pts[0].x, pts[0].x - P.x);
    Vec3i y(pts[1].y - pts[0].y, pts[2].y - pts[0].y, pts[0].y - P.y);

    // u 向量和 x y 向量的点积为 0，所以 x y 向量叉乘可以得到 u 向量
    Vec3i u = x ^ y;

    // 由于 A, B, C, P 的坐标都是 int 类型，所以 u.z 必定是 int 类型，取值范围为 ..., -2, -1, 0, 1, 2, ...
    // 所以 u.z 绝对值小于 1 意味着三角形退化了，需要舍弃
    if (std::abs(u.z) < 1) {
        return Vec3f(-1, 1, 1);
    }
    return Vec3f(1.f - (u.x + u.y) / (float)u.z, u.x / (float)u.z, u.y / (float)u.z);
}

// 自己实现的三角形光栅化函数
// 主要思路是遍历 Box 中的每一个像素，和三角形的三个点做叉乘，如果叉乘均为正，着色；有一个负数，不着色
void triangle(Vec2i* pts, TGAImage& image, TGAColor color) 
{
    // 步骤 1: 找出包围盒
        // 拿到 AABB 包围盒
    int x_min = std::min(std::min(pts[0].x, pts[1].x), pts[2].x);
    int y_min = std::min(std::min(pts[0].y, pts[1].y), pts[2].y);
    int x_max = std::max(std::max(pts[0].x, pts[1].x), pts[2].x);
    int y_max = std::max(std::max(pts[0].y, pts[1].y), pts[2].y);

    // 步骤 2: 包围盒内的每一个像素和三角形顶点连线做叉乘
    Vec2i P;
    for (P.x = x_min; P.x <= x_max; P.x++) {
        for (P.y = y_min; P.y <= y_max; P.y++) {

            // Vec3i bc_screen  = crossProduct(pts, P);
            Vec3f bc_screen = barycentric(pts, P);

            // bc_screen 某个分量小于 0 则表示此点在三角形外（认为边也是三角形的一部分）
            if (bc_screen.x < 0 || bc_screen.y < 0 || bc_screen.z < 0) {
                continue;
            }

            image.set(P.x, P.y, color);
        }
    }
}


void drawModelTriangle() {
    const int width = 200;
    const int height = 200;
    TGAImage frame(width, height, TGAImage::RGB);

    // 这个是用一个模拟光照对三角形进行着色
    Model* model = new Model(S_PATH("./obj/african_head.obj"));
    Vec3f light_dir(0, 0, -1); // 假设光是垂直屏幕的
    for (int i = 0; i < model->nfaces(); i++) {
        std::vector<int> face = model->face(i);
        Vec2i screen_coords[3];
        Vec3f world_coords[3];

        // 计算世界坐标和屏幕坐标
        for (int j = 0; j < 3; j++) {
            Vec3f v = model->vert(face[j]);
            // 投影为正交投影，而且只做了个简单的视口变换
            screen_coords[j] = Vec2i((v.x + 1.) * width / 2., (v.y + 1.) * height / 2.);
            world_coords[j] = v;
        }

        // 计算世界坐标中某个三角形的法线（法线 = 三角形任意两条边做叉乘）
        Vec3f n = (world_coords[2] - world_coords[0]) ^ (world_coords[1] - world_coords[0]);
        n.normalize(); // 对 n 做归一化处理

        // 三角形法线和光照方向做点乘，点乘值大于 0，说明法线方向和光照方向在同一侧
        // 值越大，说明越多的光照射到三角形上，颜色越白
        float intensity = n * light_dir;
        if (intensity > 0) {
            triangle(screen_coords, frame, TGAColor(intensity * 255, intensity * 255, intensity * 255, 255));
        }
    }

    //frame.flip_vertically();
    frame.write_tga_file(OUT_PATH("/day03_light_model.tga"));

    delete model;
}


int main(int argc, char** argv) {
    drawModelTriangle();
    myDrawModel();
    return 0;
}