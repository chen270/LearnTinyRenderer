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
    Triangle(t0[0], t0[1], t0[2], image, green);
    Triangle(t1[0], t1[1], t1[2], image, white);
    Triangle(t2[0], t2[1], t2[2], image, red);
    
    //image.flip_vertically();
    image.write_tga_file(OUT_PATH("lesson00.tga"));
    return 0;
}


