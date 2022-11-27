#include "tgaimage.h"
#include "model.h"

#ifndef DIR_PATH
#error SourceDir path not define!
#else
#define S_PATH(str) DIR_PATH##str
#define OUT_PATH(str) DIR_PATH##"./bin/"##str
#endif

const TGAColor red = TGAColor(255, 0, 0, 255);
const TGAColor white = TGAColor(255, 255, 255, 255);

void line(int x0, int y0, int x1, int y1, TGAImage& image, TGAColor color) {
    bool steep = false;
    if (std::abs(x0 - x1) < std::abs(y0 - y1)) { // 防止线过抖
        std::swap(x0, y0);
        std::swap(x1, y1);
        steep = true;
    }
    if (x0 > x1) { // 线段的点绘制顺序问题
        std::swap(x0, x1);
        std::swap(y0, y1);
    }
    int dx = x1 - x0;
    int dy = y1 - y0;
    int derror2 = std::abs(dy) * 2;
    int error2 = 0;
    int y = y0;
    for (int x = x0; x <= x1; x++) {
        if (steep)
            image.set(y, x, color);
        else
            image.set(x, y, color);
        error2 += derror2;
        if (error2 > dx) {
            y += (y1 > y0 ? 1 : -1);
            error2 -= dx * 2;
        }
    }
}

int main(int argc, char** argv) {
    const int width = 500;
    const int height = 500;

    TGAImage image(width, height, TGAImage::RGB);
    //image.set(52, 41, red);
    //line2(10, 10, 50, 50, image, red);
    //line(13, 20, 80, 40, image, white);
    //line(20, 13, 40, 80, image, red);
    //line(80, 40, 13, 20, image, red);

    Model* model = new Model(S_PATH("./obj/african_head.obj"));
    for (int i = 0; i < model->nfaces(); i++) {
        std::vector<int> face = model->face(i);
        for (int j = 0; j < 3; j++) {
            Vec3f v0 = model->vert(face[j]);
            Vec3f v1 = model->vert(face[(j + 1) % 3]);
            int x0 = (v0.x + 1.) * width / 2.;
            int y0 = (v0.y + 1.) * height / 2.;
            int x1 = (v1.x + 1.) * width / 2.;
            int y1 = (v1.y + 1.) * height / 2.;
            line(x0, y0, x1, y1, image, white);
        }
    }
    delete model;
    image.flip_vertically();
    image.write_tga_file(OUT_PATH("lesson00.tga"));
    return 0;
}


