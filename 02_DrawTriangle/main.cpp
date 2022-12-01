#include "tgaimage.h"
#include "model.h"
#include "basic_shape.h"

#ifndef DIR_PATH
#error SourceDir path not define!
#else
#define S_PATH(str) DIR_PATH##str
#define OUT_PATH(str) DIR_PATH##"./bin/"##str
#endif

int main(int argc, char** argv) {
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


