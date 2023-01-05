#include "tgaimage.h"
#include "model.h"
#include "basic_shape.h"

#ifndef DIR_PATH
#error SourceDir path not define!
#else
#define S_PATH(str) DIR_PATH##str
#define OUT_PATH(str) DIR_PATH##"./bin/"##str
#endif

void myDrawModel2()
{
    const int width = 200;
    const int height = 200;
    TGAImage image(width, height, TGAImage::RGB);

    Model* model = new Model(S_PATH("./obj/african_head.obj"));
    Vec3f lightDir = Vec3f(0, 0, -1);
    for (int i = 0; i < model->nfaces(); ++i) {
        std::vector<int> face = model->face(i);
        Vec2i aPoint[3];
        Vec3f world_coords[3];   // 新加入一个数组用于存放三个顶点的世界坐标
        for (int j = 0; j < 3; j++) {
            Vec3f v0 = model->vert(face[j]);
            int x0 = (v0.x + 1.) * width / 2.;
            int y0 = (v0.y + 1.) * height / 2.;
            aPoint[j] = { x0, y0 }; // 简单的视口变换
            world_coords[j] = v0;
        }

        // 这里三角形ABC 满足一定的顺序，即 AC x AB
        Vec3f n = ((world_coords[2] - world_coords[0]) ^ (world_coords[1] - world_coords[0])).normalize();
        float intensity = n * lightDir; // 光照强度=法向量*光照方向   即法向量和光照方向重合时，亮度最高
        if (intensity > 0)
        {
            // Triangle(aPoint[0], aPoint[1], aPoint[2], image, TGAColor(intensity * 255, intensity * 255, intensity * 255, 255));
            Triangle(aPoint, world_coords, image, TGAColor(intensity * 255, intensity * 255, intensity * 255, 255));
        }
    }

    //image.flip_vertically();
    image.write_tga_file(OUT_PATH("lesson00.tga"));
    delete model;
}

void myDrawModel_uv()
{
    TGAImage uvImg;
    uvImg.read_tga_file(S_PATH("./obj/african_head_diffuse.tga"));

    const int width = 200;
    const int height = 200;
    TGAImage image(width, height, TGAImage::RGB);

    Model* model = new Model(S_PATH("./obj/african_head.obj"));
    Vec3f lightDir = Vec3f(0, 0, -1);
    for (int i = 0; i < model->nfaces(); ++i) {
        std::vector<int> face = model->face(i);
        std::vector<int> face_uv = model->face_uv(i);
        Vec2i aPoint[3];
        Vec3f world_coords[3];   // 新加入一个数组用于存放三个顶点的世界坐标
        TGAColor color[3];
        for (int j = 0; j < 3; j++) {
            Vec3f v0 = model->vert(face[j]);
            int x0 = (v0.x + 1.) * width / 2.;
            int y0 = (v0.y + 1.) * height / 2.;
            aPoint[j] = { x0, y0 }; // 简单的视口变换
            world_coords[j] = v0;

            Vec2f uv_vt = model->uv_vert(face_uv[j]);
            int uv_x = (uv_vt.x + 1.) * width / 2.;
            int uv_y = (uv_vt.y + 1.) * height / 2.;
            color[j] = uvImg.get(uv_x, uv_y);
        }

        // 这里三角形ABC 满足一定的顺序，即 AC x AB
        Vec3f n = ((world_coords[2] - world_coords[0]) ^ (world_coords[1] - world_coords[0])).normalize();
        float intensity = n * lightDir; // 光照强度=法向量*光照方向   即法向量和光照方向重合时，亮度最高
        if (intensity > 0)
        {
            //Triangle(aPoint, world_coords, image, TGAColor(intensity * 255, intensity * 255, intensity * 255, 255));
            Triangle(aPoint, world_coords, image, color);
        }
    }

    //image.flip_vertically();
    image.write_tga_file(OUT_PATH("lesson00.tga"));
    delete model;
}


int main(int argc, char** argv) {
    myDrawModel_uv();
    return 0;
}