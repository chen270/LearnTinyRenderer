#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include "model.h"

Model::Model(const char *filename) : verts_(), faces_()
{
    std::ifstream in;
    in.open(filename, std::ifstream::in);
    if (in.fail())
        return;
    std::string line;
    while (!in.eof())
    {
        std::getline(in, line);
        std::istringstream iss(line.c_str());
        char trash;
        if (!line.compare(0, 2, "v "))
        {
            iss >> trash;
            Vec3f v;
            for (int i = 0; i < 3; i++)
                iss >> v.raw[i];
            verts_.push_back(v);
        }
        else if (!line.compare(0, 2, "f "))
        {
            std::vector<int> f;
            std::vector<int> vt;
            int f_vt, itrash, idx;
            iss >> trash;
            while (iss >> idx >> trash >> f_vt >> trash >> itrash)
            {
                idx--; // in wavefront obj all indices start at 1, not zero
                f.push_back(idx);
                vt.push_back(f_vt - 1);
            }
            faces_.push_back(f);
            f_vt_.push_back(vt);
        }
        else if (!line.compare(0, 2, "vt"))
        {
            char trash2;
            iss >> trash >> trash2;
            Vec2f vt;
            for (int i = 0; i < 2; i++)
                iss >> vt.raw[i];
            uv_.push_back(vt);
        }
        else if (!line.compare(0, 3, "vn ")) {
            iss >> trash >> trash;
            Vec3f normal;
            for (int i = 0; i < 3; i++) iss >> normal.raw[i];
            norms_.push_back(normal);
        }
    }
    std::cerr << "# v# " << verts_.size() << " f# " << faces_.size() << std::endl;
}

Model::~Model()
{
}

int Model::nverts()
{
    return (int)verts_.size();
}

int Model::nfaces()
{
    return (int)faces_.size();
}

std::vector<int> Model::face(int idx)
{
    return faces_[idx];
}

Vec3f Model::vert(int i)
{
    return verts_[i];
}

std::vector<int> Model::face_uv(int idx)
{
    return f_vt_[idx];
}

Vec2f Model::uv_vert(int i)
{
    return uv_[i];
}
