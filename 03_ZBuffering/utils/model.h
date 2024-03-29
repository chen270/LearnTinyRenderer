﻿#ifndef __MODEL_H__
#define __MODEL_H__

#include <vector>
#include "geometry.h"

class Model {
private:
	std::vector<Vec3f> verts_;
	std::vector<std::vector<int> > faces_;

    std::vector<Vec2f> uv_;
    std::vector<std::vector<int>> f_vt_;
	std::vector<Vec3f> norms_;
public:
	Model(const char *filename);
	~Model();
	int nverts();
	int nfaces();
	Vec3f vert(int i);
	std::vector<int> face(int idx);
	std::vector<int> face_uv(int idx);
	Vec2f uv_vert(int i);
};

#endif //__MODEL_H__